using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;

namespace V1
{
    public partial class Form1 : Form
    {
        Socket server;
        string ID_jugador;
        int port = 9092;
        string ip = "192.168.56.102";
        int conectado = 0;

        public Form1()
        {
            InitializeComponent();
        }

        private void Consulta_Button_Click(object sender, EventArgs e)
        {
            if (conectado == 0)
            {
                //Creamos un IPEndPoint con el ip del servidor y puerto del servidor 
                //al que deseamos conectarnos
                IPAddress direc = IPAddress.Parse(ip);
                IPEndPoint ipep = new IPEndPoint(direc, port);


                //Creamos el socket 
                server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                try
                {
                    server.Connect(ipep);//Intentamos conectar el socket               
                    //MessageBox.Show("Conectado");
                    conectado = 1;
                }
                catch (SocketException)
                {
                    //Si hay excepcion imprimimos error y salimos del programa con return 
                    MessageBox.Show("No he podido conectar con el servidor");
                    return;
                }
                
            }

            if (Record.Checked)
            {
                // Quiere saber quien tiene el record
                string mensaje = "3/";
                // Enviamos al servidor el codigo
                byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                server.Send(msg);

                //Recibimos la respuesta del servidor
                byte[] msg2 = new byte[80];
                server.Receive(msg2);
                mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];

                MessageBox.Show(mensaje);
            }

            else if (Personajes.Checked)
            {
                // Quiere saber que personajes se escogieron en la partida seleccionada
                string mensaje = "4/" + ID_Partida.Text;
                // Enviamos al servidor el ID de la partida tecleado
                byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                server.Send(msg);
                ID_Partida.Text = "";
                //Recibimos la respuesta del servidor
                byte[] msg2 = new byte[80];
                server.Receive(msg2);
                mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
                MessageBox.Show(mensaje);
            }

            else if (Partidas.Checked)
            {
                //Quiere cuantas partidas ha jugado el jugador seleccionado
                string mensaje = "5/" + ID_Jugador.Text;
                // Enviamos al servidor el nombre y la altura tecleados
                byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                server.Send(msg);

                //Recibimos la respuesta del servidor
                byte[] msg2 = new byte[80];
                server.Receive(msg2);
                mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];

                MessageBox.Show(mensaje);
            }
            else if (Conectados.Checked)
            {
                //Quiere saber quien esta conectado
                string mensaje = "6/";
                // Enviamos al servidor el nombre y la altura tecleados
                byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                server.Send(msg);

                //Recibimos la respuesta del servidor
                byte[] msg2 = new byte[80];
                server.Receive(msg2);
                mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];

                if (mensaje != "0")
                {
                    Form2 f2 = new Form2();
                    f2.dar_nombres(mensaje);
                    f2.Show();
                }
                else
                {
                    Form3 f3 = new Form3();
                    f3.Show();
                }
            }

        }

        private void Registrarse_Button_Click(object sender, EventArgs e)
        {
            if (conectado == 0)
            {
                //Creamos un IPEndPoint con el ip del servidor y puerto del servidor 
                //al que deseamos conectarnos
                IPAddress direc = IPAddress.Parse(ip);
                IPEndPoint ipep = new IPEndPoint(direc, port);


                //Creamos el socket 
                server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                try
                {
                    server.Connect(ipep);//Intentamos conectar el socket               
                    //MessageBox.Show("Conectado");
                    conectado = 1;
                }
                catch (SocketException)
                {
                    //Si hay excepcion imprimimos error y salimos del programa con return 
                    MessageBox.Show("No he podido conectar con el servidor");
                    return;
                }

            }
            if ((Nombre_Registro.Text == "") || (Contraseña_Registro.Text == ""))
                MessageBox.Show("No se han rellenado correctamente todos los campos");
            else
            {
                string mensaje = "2/" + Nombre_Registro.Text + "/" + Contraseña_Registro.Text;
                // Enviamos al servidor el nombre y la contraseña del tecleado
                byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                server.Send(msg);

                //Recibimos la respuesta del servidor
                //Enviará un 0 si ese usuario ya existe, sinó enviará su ID
                byte[] msg2 = new byte[80];
                server.Receive(msg2);
                mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
                if (mensaje == "0")
                    MessageBox.Show("Este usuario ya existe");
                //else if (mensaje == "0")
                // 
                else
                {
                    ID_jugador = mensaje;
                    MessageBox.Show("Te has registrado correctamente, tu ID de jugador es: " + mensaje);
                }
            }
        }

        private void Iniciar_Button_Click(object sender, EventArgs e)
        {
            if (conectado == 0)
            {
                //Creamos un IPEndPoint con el ip del servidor y puerto del servidor 
                //al que deseamos conectarnos
                IPAddress direc = IPAddress.Parse(ip);
                IPEndPoint ipep = new IPEndPoint(direc, port);


                //Creamos el socket 
                server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                try
                {
                    server.Connect(ipep);//Intentamos conectar el socket               
                    //MessageBox.Show("Conectado");
                    conectado = 1;
                }
                catch (SocketException)
                {
                    //Si hay excepcion imprimimos error y salimos del programa con return 
                    MessageBox.Show("No he podido conectar con el servidor");
                    return;
                }

            }

            string mensaje = "1/" + Nombre.Text + "/" + Contraseña.Text;
            // Enviamos al servidor el nombre y la contraseño tecleado
            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

            //Recibimos la respuesta del servidor
            //La respuesta será 0 si no se ha encontrado el usuario en labase de datos, sinó enviara su ID
            byte[] msg2 = new byte[80];
            server.Receive(msg2);
            mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
            if (mensaje == "0")
                MessageBox.Show("Este usuario no existe");
            else
            {
                ID_jugador = mensaje;
                MessageBox.Show("Se ha iniciado sesión correctamente, tu ID de jugador es: " + mensaje);
            }

        }

        private void Desconectar_Click(object sender, EventArgs e)
        {
            string mensaje = "0/";
            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

            // Nos desconectamos
            server.Shutdown(SocketShutdown.Both);
            conectado = 0;
            server.Close();
            MessageBox.Show("Desconectado");
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void No_button_Click(object sender, EventArgs e)
        {

        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {

        }
    }
}
