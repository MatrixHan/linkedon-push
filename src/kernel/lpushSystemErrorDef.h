#pragma once


#define ERROR_SUCCESS 					0

#define ERROR_SYSTEM_HANDSHAKE                     	200	
#define ERROR_FMT					201
#define ERROR_CONN_HREATBEAT_TIMEOUT			202
#define ERROR_DATA_EMPTY				203
#define ERROR_MONGODB_INSERT				204
#define ERROR_MONGODB_UPDATE				205
#define ERROR_MONGODB_DELETE				206
#define ERROR_MONGODB_FIND				207


#define ERROR_USER_SCRETE_NO_EXIST			208
#define ERROR_USER_SCRETE_MISMATCH			209
#define ERROR_DATA_MAX					210

#define ERROR_OBJECT_NOT_EXIST				211
#define ERROR_NOT_ACTIVE				212
#define ERROR_USER_IS_EXIST				213

#define ERROR_SOCKET_CREATE                 		1000
#define ERROR_SOCKET_SETREUSE               		1001
#define ERROR_SOCKET_BIND                   		1002
#define ERROR_SOCKET_LISTEN                 		1003
#define ERROR_SOCKET_CLOSED                 		1004
#define ERROR_SOCKET_GET_PEER_NAME          		1005
#define ERROR_SOCKET_GET_PEER_IP            		1006
#define ERROR_SOCKET_READ                   		1007
#define ERROR_SOCKET_READ_FULLY             		1008
#define ERROR_SOCKET_WRITE                  		1009
#define ERROR_SOCKET_WAIT                   		1010
#define ERROR_SOCKET_TIMEOUT                		1011
#define ERROR_SOCKET_CONNECT                		1012
#define ERROR_ST_SET_EPOLL                  		1013
#define ERROR_ST_INITIALIZE                 		1014
#define ERROR_ST_OPEN_SOCKET                		1015
#define ERROR_ST_CREATE_LISTEN_THREAD       		1016
#define ERROR_ST_CREATE_CYCLE_THREAD        		1017
#define ERROR_ST_CONNECT                    		1018
#define ERROR_SYSTEM_PACKET_INVALID         		1019
#define ERROR_SYSTEM_CLIENT_INVALID         		1020
#define ERROR_SYSTEM_ASSERT_FAILED          		1021
#define ERROR_READER_BUFFER_OVERFLOW        		1022
#define ERROR_SYSTEM_CONFIG_INVALID         		1023
#define ERROR_SYSTEM_CONFIG_DIRECTIVE       		1024
#define ERROR_SYSTEM_CONFIG_BLOCK_START     		1025
#define ERROR_SYSTEM_CONFIG_BLOCK_END       		1026
#define ERROR_SYSTEM_CONFIG_EOF             		1027
#define ERROR_SYSTEM_STREAM_BUSY            		1028
#define ERROR_SYSTEM_IP_INVALID             		1029
#define ERROR_SYSTEM_FORWARD_LOOP           		1030
#define ERROR_SYSTEM_WAITPID                		1031
#define ERROR_SYSTEM_BANDWIDTH_KEY          		1032
#define ERROR_SYSTEM_BANDWIDTH_DENIED       		1033
#define ERROR_SYSTEM_PID_ACQUIRE            		1034
#define ERROR_SYSTEM_PID_ALREADY_RUNNING    		1035
#define ERROR_SYSTEM_PID_LOCK               		1036
#define ERROR_SYSTEM_PID_TRUNCATE_FILE      		1037
#define ERROR_SYSTEM_PID_WRITE_FILE         		1038
#define ERROR_SYSTEM_PID_GET_FILE_INFO      		1039
#define ERROR_SYSTEM_PID_SET_FILE_INFO      		1040
#define ERROR_SYSTEM_FILE_ALREADY_OPENED    		1041
#define ERROR_SYSTEM_FILE_OPENE             		1042
#define ERROR_SYSTEM_FILE_CLOSE             		1043
#define ERROR_SYSTEM_FILE_READ              		1044
#define ERROR_SYSTEM_FILE_WRITE             		1045
#define ERROR_SYSTEM_FILE_EOF               		1046
#define ERROR_SYSTEM_FILE_RENAME            		1047
#define ERROR_SYSTEM_CREATE_PIPE            		1048
#define ERROR_SYSTEM_FILE_SEEK              		1049
#define ERROR_SYSTEM_IO_INVALID             		1050
#define ERROR_ST_EXCEED_THREADS             		1051
#define ERROR_SYSTEM_SECURITY               		1052
#define ERROR_SYSTEM_SECURITY_DENY          		1053
#define ERROR_SYSTEM_SECURITY_ALLOW         		1054
#define ERROR_SYSTEM_TIME                   		1055
#define ERROR_SYSTEM_DIR_EXISTS             		1056
#define ERROR_SYSTEM_CREATE_DIR             		1057
#define ERROR_SYSTEM_KILL                   		1058
