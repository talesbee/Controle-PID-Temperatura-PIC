// Gráfico
// por David A. Mellis
//
// Demonstra a leitura de dados vindos a partir da placa Arduino
// por meio do desenho de um gráfico dos valores recebidos.
//
// baseado em Analog In
// por <a href="http://itp.jtnimoy.com">Josh Nimoy</a>.
import processing.serial.*;
Serial port;
String buff = "";
int NEWLINE = 10;
// Armazene os últimos 64 valores recebidos de forma que possamos
//desenhá-los
int[] values = new int[64];
void setup()
{
  size(512, 256);
  println("Portas seriais disponíveis:");
  println(Serial.list());
  port = new Serial(this, Serial.list()[1], 9600);// atenção para esta
}
void draw()
{
  background(53);
  stroke(255);
  // Desenhe os valores guardados com uma linha entre os pontos.
  for (int i = 0; i < 63; i++)
    line(i * 8, 255 - values[i], (i + 1) * 8, 255 - values[i + 1]);
    
  while (port.available() > 0) {
    serialEvent(port.read());
  }
}
void serialEvent(int serial)
{
  if (serial != NEWLINE) {
    // Armazene todos os caracteres na linha
    buff += char(serial);
  } else {
    println(buff);
    buff = "";
    /*
    if (buff != "") {
      // O final de cada linha é marcado com dois caracteres: um retorno
      // de carro e um nova linha. Chegamos aqui porque temos um nova
      //linha, 
      // mas ainda precisamos excluir o retorno de carro.
      buff = buff.substring(0, buff.length()-1);
      // Transforme a string em inteiro. Dividimos por 4 porque
      // as entradas analógicas vão de 0 a 1023, enquanto que as cores
      // em Processing vão apenas de 0 a 255.
      int val = Integer.parseInt(buff);
      // Limpe o valor contido em "buff"
      
      // Desloque os valores existentes para liberar espaço para novos
      //valores.
      for (int i = 0; i < 63; i++)
        values[i] = values[i + 1];
      // Inclua ao vetor o valor recebido
      values[63] = val;
    }*/
    
  }
}
