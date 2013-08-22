/*
Features
 * This sketch receives the command via COM interface and drive the three stepper motors and on motor.

Steps to deploy this sketch
 1. Configure the sketch as the Configuration section.
 2. Build and upload the sketch to aruino board. (You need to copy all the libs under https://github.com/JeffreyZksun/easyrobot/tree/master/libs to <Arduino>/Libraries/ folder)
 3. Wire the harwares as the Wire section.
 4. Power up the arduino board.

Test local commands
 * Help: "0x50 0x02 0xFA 0x01\r\n" (Route message to 0x02 from 0xFA to execute command 0x01.)
 * Drive one stepper: "0x50 0x02 0xFA 0xA0 1 1 800\r\n"
 * Drive two steppers: "0x50 0x02 0xFA 0xA0 1 1 800 2 1 1600\r\n"
 * Drive three steppers: "0x50 0x02 0xFA 0xA0 1 1 800 2 1 1600 3 2 600\r\n"

Configuration
 * Set MY_ADDRESS. This node is not in the network. Any address is ok.
 * Set ROUTER_RELAY_ENABLE. If the Ardunio connects to XBee coordinator, set it true. Otherwise, set it false.

Hardwares
    UNO 
    EasyDriver v4.4
    
Services
    System
    UARTCmdReceiver
    MessageRouter


Circuit:
    Stepper  <-->  EasyDriver
        Stepper	EasyDriver
        Black	A
        Green	A-
        Red	    B
        Blue	B-
        
    EasyDriver  <-->  Arduino 
        EasyDriver #1	Arduino
        GND	            GND
        STEP	        3
        DIR	            2
        
        EasyDriver #2	Arduino
        GND	            GND
        STEP	        5
        DIR	            4
        
        EasyDriver #3	Arduino
        GND	            GND
        STEP	        7
        DIR	            6
        
    Power
        Connect EasyDriver PWR IN pins to 6~30V power source.
        Connect the Arduino USB port to 5V power source. (You can use other power pins.)
    

Requried library
	ERxXxxx : https://github.com/JeffreyZksun/easyrobot/tree/master/libs
    AccelStepper

* Created 20 August 2012
* By Jeffrey Sun
* 

*/

#include <Arduino.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <ERxIOStream.h>
#include <ERxOutputStream.h>
#include <ERxTextMessage.h>
#include <ERxHost.h>
#include <ERxService.h>
#include <ERxSystemService.h>
#include <ERxUARTCmdReceiverService.h>
#include <ERxMessageRouterService.h>
#include <AccelStepper.h>


//--------------Define the stepper motor driver service----------------------------------
// Define the pins which connect the easy driver.
#define SREPPER_1_STEP_PIN 3
#define SREPPER_1_DIR_PIN 2
#define SREPPER_2_STEP_PIN 5
#define SREPPER_2_DIR_PIN 4
#define SREPPER_3_STEP_PIN 7
#define SREPPER_3_DIR_PIN 6

// Define the command id.
#define DIRVE_STEPPER_NOTOR_CMD 0xA0

class ERxStepperMotorDrivingService : public ERxService
{
public:
	ERxStepperMotorDrivingService(): m_stepper1(1, SREPPER_1_STEP_PIN, SREPPER_1_DIR_PIN)
                                    , m_stepper2(1, SREPPER_2_STEP_PIN, SREPPER_2_DIR_PIN)
                                    , m_stepper3(1, SREPPER_3_STEP_PIN, SREPPER_3_DIR_PIN)
    {
    }

public:
	// Generate the command based on the current status of the service. 
	// Set it to context if there is. 
	virtual void Populate(ERxServiceContext& context)
    {
        m_stepper1.run();
        m_stepper2.run();
        m_stepper3.run();        
    }

	// Execute the command saved in the command.
	virtual void Execute(ERxServiceContext& context)
    {

        switch(context.GetCommandId())
        {
        case DIRVE_STEPPER_NOTOR_CMD:
        {
            // Command Format 
            // <STEPPER_ID> 1|2|3
            // <DIR> 1 means positive, 2 means negative.
            // <DIRVE_STEPPER_NOTOR_CMD><SPACE><STEPPER_ID><SPACE><DIR><SPACE><DISTANCE>[<SPACE><STEPPER_ID><<SPACE><DIR>SPACE><DISTANCE>][<SPACE><STEPPER_ID><SPACE><DIR><SPACE><DISTANCE>]
            // For example: 1 1 800 2 1 1600 3 2 600
            
            // There are at most three instructions
            bool hasInstruction = DriveOneStepperMotor(context.GetCommandParameterStream());
            
            if(hasInstruction)
                hasInstruction = DriveOneStepperMotor(context.GetCommandParameterStream());
            
            if(hasInstruction)
                hasInstruction = DriveOneStepperMotor(context.GetCommandParameterStream());
            
            
            context.SetIsCommandExecuted(true);
			context.SetIsCommandSuccess(true);
        }   
            break;        
        default:
            // Do nothing
            break;
        }
    }

	// Print the help message of this service. 
	virtual void PrintHelp(ERxServiceContext& context)
    {
        ERxOutputStream& rResultStream = context.GetResultStream(); 

        PROG(serviceName) = ">> ERxStepperMotorDrivingService \r\n";
        rResultStream.printP(serviceName);
    }

	// Response the result. 
	virtual void HandleResult(ERxServiceContext& context){}
    
private:
    bool DriveOneStepperMotor(ERxIOStream& parameterStream)
    {
        const unsigned long id = parameterStream.parseULong();
        if(!id)
            return false;
        const unsigned long dir = parameterStream.parseULong();
        if(!dir)
            return false;
        long dist = (long) parameterStream.parseULong();
        
        if(dir == 2) // Reverse the direction
            dist = -dist;
               
        switch(id)
        {
            case 1:
                m_stepper1.move(dist);
                break;
            case 2:
                m_stepper2.move(dist);
                break;                
            case 3:
                m_stepper3.move(dist);
                break;       
        }
        
        return true;    
    }

private:
	AccelStepper    m_stepper1;
    AccelStepper    m_stepper2;
    AccelStepper    m_stepper3;
};

//--------------CONFIGURATION-------------------------------------------------------------
// Macro definitions which require modifying for different harwares.
//
#define MY_ADDRESS 0x02
// ***IMPORTANT*** :Set true on the coordinator, and false on end device.
#define ROUTER_RELAY_ENABLE false

//----------------------------------------------------------------------------------------
// Allocate the buffers
#define COMMAND_PARAMETER_BUFFER_SIZE 128
#define RESULT_BUFFER_SIZE 128
static uint8_t sCmdBuffer[COMMAND_PARAMETER_BUFFER_SIZE];
static uint8_t sResultBuffer[RESULT_BUFFER_SIZE];

// XBee
// AES encryption dictates that the maximum RF packet size is 95 Bytes.
// The XBee buffer size is 95. To make sure the message sent to XBee is packed into a package, this buffer should be less than 95.
#define ROUTER_BUFFER_SIZE 90
static uint8_t sRouterBuffer[ROUTER_BUFFER_SIZE];

// Define the host and the supported services
ERxHost host(sCmdBuffer, COMMAND_PARAMETER_BUFFER_SIZE, sResultBuffer, RESULT_BUFFER_SIZE);
ERxSystemService sysService(&host);
ERxUARTCmdReceiverService uartService(&Serial);
ERxMessageRouterService routerService(&host, &Serial, sRouterBuffer, ROUTER_BUFFER_SIZE);
ERxStepperMotorDrivingService stepperService;

void setup() {

	Serial.begin(9600);

	// Set the address.
	host.SetMyAddress(MY_ADDRESS);
	routerService.SetRelayMessage(ROUTER_RELAY_ENABLE); // IMPORTANT: Only set true on the coordinator

	// Add service to host.
	host.AddService(&sysService);
	host.AddService(&uartService);
	host.AddService(&routerService);
    host.AddService(&stepperService);

	Serial.println("System startup");
}

void loop() {

	host.Run();
}