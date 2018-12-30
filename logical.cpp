
/* 
Created by UTKARSH MEHTA on 20/November/2018.

Copyright � 2018 Demo. All rights reserved.
UTKARSHMEHTA93@GMAIL.COM
Advanced Operating Systems
Fall 2018
Dr.Belkhoche
LOGICAL CLOCK
MAIN.CPP */

 
 /* NOTES: If you are running for 2 processes that is the first line in the input file is 2, then the command for command line on putty will be np-2.
If you are running for 5 processes that is the first line in the input file is 5, then the command for command line on putty will be np-5. */
 
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <string>
using namespace std;

#define requestTag  50
#define logicalClockCounterTag 51
#define debug 0

/*
 This method is used to initialise MPI Message Passing Interface.
 */
void initialiseMPI(int *argc, char *argv[], int *rank, int *size) {

    MPI_Init(argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, rank);
    MPI_Comm_size(MPI_COMM_WORLD, size);
}

int main(int argc, char *argv[]) {


    //Variable Declartions
    int logicalClockCounter = 0, processes=0;
    int rank;

    //Initialising MPI
    initialiseMPI(&argc, argv, &rank, &processes);

    string inputline("");

    char *fileName = argv[1];



    if(fileName == NULL) {
        cout<<"Please check the arguments";
        return 0;
    }

    if( rank == 0)
        cout<<"There are "<< processes <<"  processes in the system "<<endl;

    ifstream fileObject(fileName);

    if(fileObject.is_open()) {

        if (debug)
            cout<<"File has been opened";

        while (!fileObject.eof())  {

            getline(fileObject, inputline);

            if(debug)
                cout<<"Data Read From File....."<<inputline;

            if (inputline.find("exec") != std::string::npos)
            {
                if(debug)
                    cout<<"Inside the Execute Condition";

                string processNumber = inputline.substr(5,1);
                int pid;
                istringstream(processNumber) >> pid;
                int rankOfProcess = pid - 1;

//                if(debug)
//                    cout<<"Character REad is ....."<<character[0];

//                int processNumber = atoi(character);
//                int rankOfProcess = processNumber - 1;

                if(debug)
                    cout<<"Process Number is ......"<< pid;

                //Execute Command for the process is the same Process executing
                if(rankOfProcess == rank) {
                    logicalClockCounter++;

                    cout << "Execution event in process " << rank <<endl;
                    cout << "Logical time at process  " << rank << " is " << logicalClockCounter << endl;
                }

            } else if (inputline.find("send") != std::string::npos ) {

                if(debug)
                    cout<<"Inside the Send Condition";


                string message("");
                string requestFrom = inputline.substr(5, 1);
                string requestTo = inputline.substr(7, 1);
                message = inputline.substr((inputline.find('\"')+1), ((inputline.length())- (inputline.find('\"')+2)));

                int sentFrom, sentTo;
                istringstream(requestFrom) >> sentFrom;
                istringstream(requestTo) >> sentTo;

                //                int sentFrom = boost::lexical_cast<int>(requestFrom);
                //                int sentTo = boost::lexical_cast<int>(requestTo);

                if(debug) {

                    cout<<"Sent From Valiue  is ...."<<sentFrom<<endl;
                    cout<<"Sent To  Value  is ...."<<sentTo<<endl;
                }


                int sentToRank =  sentTo - 1;
                int sentFromRank = sentFrom - 1 ;

                if(debug) {

                    cout<<"Sent From  Value is .."<< sentFromRank <<endl;
                    cout<<"Sent To Value is .."<< sentToRank<<endl;
                    cout<<"message is .."<<message<<endl;
                }


                if(sentFromRank == rank) {

                    logicalClockCounter++;

                    MPI_Send(message.c_str(), 100 ,MPI_CHAR, sentToRank, requestTag, MPI_COMM_WORLD);
                    MPI_Send(&logicalClockCounter, 1, MPI_INT, sentToRank, logicalClockCounterTag, MPI_COMM_WORLD);

                    cout<<"Message sent from process "<< rank <<" to process " << sentToRank <<": " <<message <<endl;
                    cout << "Logical time at process  " << rank << " is:\t " << logicalClockCounter << endl;
                }


                //If it is the process to which the Message has been sent it will receive the message
                if(sentToRank == rank ) {

                    if(debug)
                        cout<<"In Received Condition";

                    char receivedMessage[100];
                    MPI_Status status;
                    int receivedLocalCounter;

                    MPI_Recv(&receivedLocalCounter, 1, MPI_INT, sentFromRank, logicalClockCounterTag, MPI_COMM_WORLD, &status);
                    MPI_Recv(&receivedMessage, 100, MPI_CHAR, sentFromRank, requestTag, MPI_COMM_WORLD, &status);
                    cout<<"Message received from process "<< sentFromRank <<" by process " << rank <<": " <<receivedMessage <<endl;

                    int maxLogicalCounter = (receivedLocalCounter>logicalClockCounter)? receivedLocalCounter: logicalClockCounter;
                    logicalClockCounter = maxLogicalCounter + 1;
                    cout << "Logical time at process  " << rank << " is:\t " << logicalClockCounter << endl;

                }
            }
//            MPI_Finalize();

            MPI_Barrier(MPI_COMM_WORLD);
        }
    }else {

        cout<<"Unable to open the file";
        return 0;
    }

    MPI_Finalize();
}
