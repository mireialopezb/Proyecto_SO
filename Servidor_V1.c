#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <mysql.h>
#include <ctype.h>
#define port 9068

int main(int argc, char *argv[])
{
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char buff[512];
	char buff2[512];
	
	MYSQL *conn;
	int err;
	//estructura especial para almacenar resultados de consultas
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	int rondas ,ID_ganador, duracion;
	int rondas_record=1000, ID_ganador_record=1000,duracion_record=100000;
	char consulta [80];
	
	//creamos una conexión al servidor MYSQL
	conn=mysql_init(NULL);
	if(conn==NULL)
	{
		printf("Error al crear la conexión: %u %s\n",
			   mysql_errno(conn),mysql_error(conn));
		exit(1);
	}
	
	//inicializar la conexión
	conn =mysql_real_connect(conn,"localhost","root","mysql","juego",0,NULL,0);
	if(conn==NULL)
	{
		printf("Error al iniciar la conexión: %u %s\n",
			   mysql_errno(conn), mysql_error(conn));
		exit(1);
	}
	
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// escucharemos en el port 
	serv_adr.sin_port = htons(port);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	//La cola de peticiones pendientes no podr? ser superior a 4
	if (listen(sock_listen, 2) < 0)
		printf("Error en el Listen");
	
	
	// Atenderemos solo 7 peticione
	//for(int i=0;i<7;i++){
	
	for(;;)
	{ //bucle infinito
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexi?n\n");
		//sock_conn es el socket que usaremos para este cliente
		
		//Bucle de atención al cliente
		int terminar=0;
		while (terminar ==0)
		{
			
			// Ahora recibimos el código, que dejamos en buff
			ret=read(sock_conn,buff, sizeof(buff));
			printf ("Recibido\n");
			
			// Tenemos que a?adirle la marca de fin de string 
			// para que no escriba lo que hay despues en el buffer
			buff[ret]='\0';
			
			//Escribimos el nombre en la consola
			
			printf ("Se ha conectado: %s\n",buff);
			
			
			char *p = strtok( buff, "/");
			int codigo =  atoi (p);
			char nombre[20],contrasena[20],ID_jugador[10], ID_partida[10];
			
			if (codigo == 0)
				terminar=1;
			
			else if (codigo ==1) //iniciar sesion
			{
				p = strtok( NULL, "/");
				strcpy (nombre, p);
				p = strtok( NULL, "/");
				strcpy (contrasena, p);
				
				// construimos la consulta SQL
				err=mysql_query(conn,"SELECT * from jugador");
				if (err!=0)
				{
					printf("Error al consultar datos de la base %u %s\n",
						   mysql_errno(conn),mysql_error(conn));
					exit(1);
				}
				
				//recogemos el resultado de la consulta 
				resultado=mysql_store_result(conn);
				//Estructura matricial en memoria
				//cada fila contiene los datos de una partida
				
				//obtenemos los datos de una fila
				row=mysql_fetch_row(resultado);
				int encontrado=0;
				if (row==NULL)
					printf("No se han obtenido datos en la consulta\n");
				else
					while ((row !=NULL)&&(encontrado==0))
				{
						//recorre la base de datos para ver si el usuario existe
						if((strcmp(nombre,row[1])==0)&&(strcmp(contrasena,row[2])==0))
						{
							sprintf(buff2,row[0]);
							encontrado=1;
						}
						row=mysql_fetch_row(resultado);
				}
					if (encontrado==0) //si no ha encontrado al usuario en la base de datos, envia un 0
						sprintf(buff2,"0\0");
			}
			
			else if (codigo==2) //registrarse
			{
				p = strtok( NULL, "/");
				strcpy (nombre, p);
				p = strtok( NULL, "/");
				strcpy (contrasena, p);	
				
				// construimos la consulta SQL
				err=mysql_query(conn,"SELECT * from jugador");
				if (err!=0)
				{
					printf("Error al consultar datos de la base %u %s\n",
						   mysql_errno(conn),mysql_error(conn));
					exit(1);
				}
				
				//recogemos el resultado de la consulta 
				resultado=mysql_store_result(conn);
				//Estructura matricial en memoria
				//cada fila contiene los datos de una partida
				
				//obtenemos los datos de una fila
				row=mysql_fetch_row(resultado);
				int encontrado=0;
				if (row==NULL)
					printf("No se han obtenido datos en la consulta\n");
				else
				{
					while ((row !=NULL)&&(encontrado==0))
					{
						//miramos si ya existe un jugador en la base de datos con el mismo nombre y contraseña
						if((strcmp(nombre,row[1])==0)&&(strcmp(contrasena,row[2])==0))
						//el jugador ya existe
						//envia un 0 al cliente para informar de que este jugador ya existe
						{
							strcpy(buff2,"0\0");
							encontrado=1;
						}
					row=mysql_fetch_row(resultado); //recorre toda la tabla
					}
					if (encontrado==0) 
					//el jugador no existe, asi que lo añade a la base de datos
					{
						//como los ID van en orden (ej: 1,2,3,4...)
						//contamos cuantos jugadores hay registrados
						//el último id usado será igual al número de jugadores
						strcpy (consulta,"SELECT COUNT(*) FROM jugador");
												
						err=mysql_query (conn, consulta);
						if (err!=0) 
						{
							printf ("Error al consultar datos de la base %u %s\n",
									mysql_errno(conn), mysql_error(conn));
							exit (1);
						}
						
						
						resultado = mysql_store_result (conn);
						row = mysql_fetch_row (resultado);
						if (row == NULL)
							printf ("No se han obtenido datos en la consulta\n");
						else
							// la columna 0 contiene el ulitimo ID de jugador usado
								sprintf(ID_jugador,"%d",atoi(row[0])+1);
						
						//creamos la consulta
						char consulta [80];
						strcpy (consulta, "INSERT INTO jugador VALUES (");
						//concatenamos el ID_jugador 
						strcat (consulta, ID_jugador); 
						strcat (consulta, ",'");
						//concatenamos el nombre 
						strcat (consulta, nombre); 
						strcat (consulta, "','");
						//concatenamos la contraseña
						strcat (consulta, contrasena); 
						strcat (consulta, "');");
						printf("%s",consulta);
						// Ahora ya podemos realizar la insercion 
						err = mysql_query(conn, consulta);
						if (err!=0) 
							printf ("Error al introducir datos la base %u %s\n", 
									mysql_errno(conn), mysql_error(conn));
						else
							//la inserción se ha realizado con exito
							//informamos al cliente enviando el ID_jugador asignado
							sprintf(buff2,"%s\0",ID_jugador);
							
					}
				}
			}
				
			else if (codigo==3) 
			//record
			{
				err=mysql_query(conn,"SELECT * from partida");
				if (err!=0)
				{
					printf("Error al consultar datos de la base %u %s\n",
						   mysql_errno(conn),mysql_error(conn));
					exit(1);
				}
				
				//recogemos el resultado de la consulta 
				resultado=mysql_store_result(conn);
				//Estructura matricial en memoria
				//cada fila contiene los datos de una partida
				
				//obtenemos los datos de una fila
				row=mysql_fetch_row(resultado);
				
				int i=0;
				if (row==NULL)
					printf("No se han obtenido datos en la consulta\n");
				else
					while (row !=NULL)
				{
						//guarda los valores de row en sus variables correspondientes
						//convirtiendolas a enteros
						duracion  =atoi(row[3]);
						ID_ganador = atoi (row[4]);
						rondas = atoi (row [5]);
						
						
						//compara los datos con los guardados en la variable
						//del jugador que mantiene el record
						if(rondas<rondas_record)
						{
							rondas_record = rondas;
							ID_ganador_record = ID_ganador;
							duracion_record = duracion;
						}
						else if((duracion<duracion_record)&&(rondas_record==rondas))
						{
							rondas_record = rondas;
							ID_ganador_record = ID_ganador;
							duracion_record = duracion;
						}
						
						row=mysql_fetch_row(resultado);
						   
				}
					
					//convertimos ID_ganador_record en char para poder hacer la consulta
					char ID[20];
					sprintf(ID, "%d", ID_ganador_record);
					
					// construimos la consulta SQL
					strcpy (consulta,"SELECT nombre FROM jugador WHERE ID_jugador = '"); 
					strcat (consulta, ID);
					strcat (consulta,"'");
					
					//hacemos la consulta
					err=mysql_query(conn, consulta);
					if(err!=0)
					{
						printf ("Error al consultar datos de la base %u %s\n",
								mysql_errno(conn), mysql_error(conn));
						exit (1);
					}
					
					//recogemos el resultado de la consulta
					resultado=mysql_store_result(conn);
					row=mysql_fetch_row(resultado);
					if(row==NULL)
						printf("No se han obtenido datos de la consulta \n");
					else
						//matriz con una fila y una columna
						sprintf(buff2,"%s tiene el record con %d rondas.\0",row[0],rondas_record);
						printf("%s tiene el record con %d rondas\n",row[0],rondas_record);
			}
				
			else if (codigo==4)
			//ID de los personajes
			{
					p = strtok( NULL, "/");
					strcpy (ID_partida, p);
					
					err=mysql_query(conn,"SELECT * from registro");
					if (err!=0)
					{
						printf("Error al consultar datos de la base %u %s\n",
							   mysql_errno(conn),mysql_error(conn));
						exit(1);
					}
					
					//recogemos el resultado de la consulta 
					resultado=mysql_store_result(conn);
					//Estructura matricial en memoria
					//cada fila contiene los datos de una partida
					
					//obtenemos los datos de una fila
					row=mysql_fetch_row(resultado);
					int ID_personaje[10],i=0;
					
					if (row==NULL)
						printf("No se han obtenido datos en la consulta\n");
					else
					{
						while (row !=NULL)
						{
							//guarda los valores de row en sus variables correspondientes
							//convirtiendolas a enteros
							if(atoi(row[0])==atoi(ID_partida))
							{
								ID_personaje[i] = atoi(row[2]);
								i++;
							}
							row=mysql_fetch_row(resultado);
						}
						// El resultado debe ser una matriz con una sola fila
						// y una columna que contiene el nombre
						for(int j=0; j<i; j++)
						{
							char personaje [10];
							sprintf(personaje, "%d", ID_personaje[j]);
							printf("%s", personaje);
							strcpy (consulta,"SELECT nombre_personaje FROM personaje WHERE ID_personaje = '");
							strcat (consulta, personaje);
							strcat (consulta,"'");
							// hacemos la consulta 
							err=mysql_query (conn, consulta); 
							if (err!=0) {
								printf ("Error al consultar datos de la base %u %s\n",
										mysql_errno(conn), mysql_error(conn));
								exit (1);
							}	
							//recogemos el resultado de la consulta 
							resultado = mysql_store_result (conn); 
							row = mysql_fetch_row (resultado);
							if (row == NULL)
								printf ("No se han obtenido datos en la consulta\n");
							else
							{
								char frase [100];
								sprintf(frase,"Nombre del personaje %d: %s.", j+1, row[0]);
								strcat (buff2,frase);
							}
							
						} 
						strcat(buff2,"\0");
					
					}
				}
				
			else if (codigo==5)
			//Cuantas partidas ha jugado un jugador
			{
				p = strtok( NULL, "/");
				strcpy (ID_jugador, p);
					
					
				char consulta [80];
				strcpy (consulta,"SELECT COUNT(*) FROM registro WHERE ID_jugador = '");
				strcat (consulta, ID_jugador);
				strcat (consulta,"'");
				
				
				err=mysql_query (conn, consulta);
				if (err!=0) {
					printf ("Error al consultar datos de la base %u %s\n",
							mysql_errno(conn), mysql_error(conn));
					exit (1);
				}
				
				resultado = mysql_store_result (conn);
				row = mysql_fetch_row (resultado);
				if (row == NULL)
					printf ("No se han obtenido datos en la consulta\n");
				else
					while (row !=NULL) 
					{
						// la columna 0 contiene el ID del jugador
						sprintf (buff2,"Ha jugado %s partidas\0", row[0]);
						row = mysql_fetch_row (resultado);
					}
				
			}
				
			if(codigo!=0)
			{
				printf ("%s\n", buff2);
				// Y lo enviamos
				write (sock_conn,buff2, strlen(buff2));
				strcpy(buff2,"");
			}
				
				
				
				
		}
		// Se acabo el servicio para este cliente
		close(sock_conn);
		mysql_close(conn);
		exit(0);
	}
}

