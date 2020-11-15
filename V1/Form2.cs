using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace V1
{
    public partial class Form2 : Form
    {
        public Form2()
        {
            InitializeComponent();
        }
        public void dar_nombres(string mensaje)
        {
            Conectados_label.Text = mensaje;
        }

        private void Cerrar_Click(object sender, EventArgs e)
        {
            Close();
            MessageBox.Show("¡De nada!");
        }

        
    }
}
