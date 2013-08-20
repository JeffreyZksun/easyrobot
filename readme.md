# Overview 
This is a service oriented framework for the Arduino hardware platform. The purposes of this framework are
 * The functionalities of Arduino are built from building blocks. 
 * The open/closed principle is satisfied when extend the functionalities. (Open for extension, but closed for modification.) 
 * Each functionality unit can be debug and test independently and easily.

# Framework
The functionalities are split into small independent services. A service is the smallest granularity. All application logics are encapsulated in the services.

The work we want Arduino to do is abstracted as the commands. The command is presented by a command id, which is a digital number, and a command parameter, which is a string. The execution result is presented by a string.  The command id, command  parameter and execution result constitute the command protocol. There is a global command id table, which are referred to by all the services. The command parameter and execution result are strings. Their schemas are defined by all the services being interested in the command. 

The framework works in the mean of command population, execution and result dispatch. Both the generator and executor of the command are services. A service is only interested in a few commands. And it only responses to the command it is interested in. A service can be just a command generator or just a executor or both. The services check if a command is it is interested by the command id. 

All the services running on the Arduino are hosted by a host. The host works as a state machine which consists of three states, which are command population state, command execution state and result dispatch state. The state machine starts from the command population state, it traverses all the services to populate a command. Once a command is populated, it PAUSEs the current states and enter into the command dispatch state. In this state, it traverses all the services to execute the command.  Once the command is labelled handled by any service, this state is STOPPED. If there is no execution result, RESUME the population state. If there is execution result, enter into the result dispatch state. In this state, it dispatches the result to all the services, then EXIT current state and RESUME the population state.

In the population state, the service detects the current status and make the decision if it should populate a command. The populated command may be executed by itself or by other services. In the execution state, the service detects if it should response the current command by checking the command id. If yes, it executes the corresponding functionalities. And generate the execution result. In the result dispatch state, the service detects if the result is that it is interested in by checking the command id.


# Arduino network
The framework can implement the Arduino network easily. 

Suppose there are three Arduino with XBee S1 stacked. Arduino 1 works as the coordinator of the network. Arduino 2 and Arduino 3 works as end devices. Arduino 2 wants to populate a remote command to be executed by Arduino 3. 

To implement this, we runs the services on each Arduino as below. 
 * Arduino 1 (A1)
   * ERxSystemService
   * ERxUARTCmdReceiverService
   * ERxMessageRouterService (enable relay)
*Note*: The three services are provided by the framework.

 * Arduino 2 (A2)
    * ERxSystemService
   * ERxUARTCmdReceiverService
   * ERxMessageRouterService (disable relay)
   * User service to populate the command. 

 * Arduino 3 (A3)
    * ERxSystemService
   * ERxUARTCmdReceiverService
   * ERxMessageRouterService (disable relay)
   * User service to execute the command. 

Power up the three devices. On Arduino 2, the user service populates a remote command in population state, which is a local command wrapped by the router prefix. In the  execution state, ERxMessageRouterService (A2) detects it is a router command. It send it to the coordinator, Arduino 1. The  ERxUARTCmdReceiverService(A1) monitors the UART and detects the command. It populates the command. In the execution state, the ERxMessageRouterService(A1) detects it is a router command, and the destination isn't itself. It dispatches this command to the end devices. The ERxUARTCmdReceiverService(A3) running on Arduino 3 receives and populates the command. In the execution state, the ERxMessageRouterService(A3) detects it is a router command, and the destination is itself. It removes the router prefix, populates a new local command and delegates the host to execute the command. The user service running on A3 detects it is a command it is interested in. It executes it and output the execution result. Since this is a remote command, the result is redirected to the A1 with router prefix. ERxUARTCmdReceiverService(A1) receives the result and populates it. ERxMessageRouterService(A1) dispatches the result to end devices. ERxUARTCmdReceiverService(A2) receives the result and populates it. ERxMessageRouterService(A2) detects the destination is this device. It populates a system result command, and ERxSystemService(A2) will dispatch the result. At last, the user service on A2 receives the execution result. 