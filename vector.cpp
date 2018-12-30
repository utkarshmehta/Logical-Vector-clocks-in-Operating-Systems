/* 
Created by UTKARSH MEHTA on 20/November/2018.

Copyright � 2018 Demo. All rights reserved.
UTKARSHMEHTA93@GMAIL.COM
Advanced Operating Systems
Fall 2018
Dr.Belkhoche
VECTOR CLOCK
MAIN.CPP */
/* NOTES: If you are running for 2 processes that is the first line in the input file is 2, then the command for command line on putty will be np-2.
If you are running for 5 processes that is the first line in the input file is 5, then the command for command line on putty will be np-5.
 */


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

    int processes=0;

    int rank;



    //Initialising MPI

    initialiseMPI(&argc, argv, &rank, &processes);



    string inputline("");



    char *fileName = argv[1];



    int vecotrClock[processes];



    for (int i = 0; i< processes; i++) {

        vecotrClock[i] = 0;

    }



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



        while ( !fileObject.eof())  {



            getline(fileObject, inputline);



            if(debug)

                cout<<"Data Read From File....."<<inputline;



            if (inputline.find("exec") != std::string::npos)

            {

                if(debug)

                    cout<<"Inside the Execute Condition";



                int pid;

                string processNumber = inputline.substr(5, 1);

                istringstream(processNumber) >> pid;

                int rankOfProcess = pid - 1;





//                if(debug)

//                    cout<<"Character REad is ....."<<character[0];



                if(debug)

                    cout<<"Process Number is ......"<< rankOfProcess;



                //Execute Command for the process is the same Process executing

                if(rankOfProcess == rank) {

                    vecotrClock[rank]  =  vecotrClock[rank] + 1;



                    cout << "Execution event in process " << rank <<endl;

                    cout << "Vector time at process  " << rank << " is " << vecotrClock[rank] << endl;

                    cout <<"Vector Value is   ";



                    for (int i =0; i<processes; i++) {

                        cout<<vecotrClock[i]<<"  ";

                    }

                    cout<<endl;



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



                    vecotrClock[rank]  =  vecotrClock[rank] + 1;



                    cout<<"Message sent from process "<< rank <<" to process " << sentToRank <<": " <<message <<endl;

                    cout << "Vector time at process  " << rank << " is:\t " << vecotrClock[rank] << endl;

                    cout <<"Vector Value is   ";

                    for (int i =0; i<processes; i++) {

                        cout<<vecotrClock[i]<<"  ";

                    }

                    cout<<endl;



                    MPI_Send(message.c_str(), 100 ,MPI_CHAR, sentToRank, requestTag, MPI_COMM_WORLD);

                    MPI_Send(vecotrClock, processes, MPI_INT, sentToRank, logicalClockCounterTag, MPI_COMM_WORLD);





                }





                //If it is the process to which the Message has been sent it will receive the message

                if(sentToRank == rank ) {



                    if(debug)

                        cout<<"In Received Condition";



                    char receivedMessage[100];

                    MPI_Status status;

                    int receivedVectorClock[processes];



                    MPI_Recv(&receivedVectorClock, processes, MPI_INT, sentFromRank, logicalClockCounterTag, MPI_COMM_WORLD, &status);

                    MPI_Recv(&receivedMessage, 100, MPI_CHAR, sentFromRank, requestTag, MPI_COMM_WORLD, &status);



                    vecotrClock[rank]  =  vecotrClock[rank] + 1;



                    for (int i = 0; i < processes; i++) {

                        vecotrClock[i] = vecotrClock[i]>receivedVectorClock[i]? vecotrClock[i]: receivedVectorClock[i];

                    }

                    cout<<"Message received from process "<< sentFromRank <<" by process " << rank <<": " <<receivedMessage <<endl;

                    cout << "Vector time at process  " << rank << " is:\t " << vecotrClock[rank] << endl;

                    cout <<"Vector Value is   ";

                    for (int i =0; i<processes; i++) {

                        cout<<vecotrClock[i]<<"  ";

                    }

                    cout<<endl;



                }

            }



            MPI_Barrier(MPI_COMM_WORLD);



        }

    }else {



        cout<<"Unable to open the file";

        return 0;

    }



    MPI_Finalize();

}