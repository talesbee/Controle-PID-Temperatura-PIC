#include <16F877A.h>

#device ADC=10

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O

#use delay(crystal=20000000)

#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8,stream=Serial)

#ifndef lcd_enable                                                             
   #define lcd_enable     pin_e1      // pino enable do LCD
   #define lcd_rs         pin_e2      // pino rs do LCD
   #define lcd_d4         pin_d4      // pino de dados d4 do LCD
   #define lcd_d5         pin_d5      // pino de dados d5 do LCD
   #define lcd_d6         pin_d6      // pino de dados d6 do LCD
   #define lcd_d7         pin_d7      // pino de dados d7 do LCD
#endif

#include <mod_lcd.c>

long leituraADC(int canal){
   set_adc_channel(canal);
   delay_us(10);
   return read_adc();
}

int passos = 0;

signed int16 
   temperaturaLida = 0.0,
   temperaturaUsuario = 0.0,
   ultimaTemperatura = 0.0,
   controlePID = 0.0,
   erro = 0.0,
   p = 0.0,
   i = 0.0,
   d = 0.0,
   kp = 1,
   ki = 1,
   kd = 1,
   tempoEmCadaLeitura = 0;
float to = 0.0;


#INT_RTCC
void  RTCC_isr(void) 
{
   /*
   Configuração do PWM para 1s -> 1000ms
   A interrupção leva 0,0512ms para ser acionada
   Se queremos que o PWM tenha 1000ms de tempo total
   Dividimos isso pelo tempo que leva cada "ciclo" (valor declarado ao criar o timer0, linha 87)
   1000ms/0,0512ms = 19.53 -> 20 ciclos, ou 20 passos
   */

   if(passos <= to){
      output_high(PIN_C5);
   }else{
      output_low(PIN_C5);
   }
   
   passos++;
   if(passos == 20){
      passos = 0;        
   }
   
}

void main()
{
   setup_adc_ports(AN0_AN1_AN2_AN3_AN4);
   set_adc_channel(0);
   delay_us(10);
   
   lcd_ini();
   
   delay_us(10);
   
   
   setup_adc(ADC_CLOCK_DIV_16);
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1|RTCC_8_bit);      //51,2 us overflow
   setup_timer_2(T2_DIV_BY_16,255,1);      //819 us overflow, 819 us interrupt

   setup_ccp1(CCP_PWM);
   set_pwm1_duty((int16)510);

   enable_interrupts(INT_RTCC);
   enable_interrupts(GLOBAL);

   while(TRUE)
   {
      //Entradas
      //0 - 1024
      temperaturaLida = leituraADC(2)/2;
      temperaturaUsuario = leituraADC(0);
      set_pwm1_duty(leituraADC(1));
      //Convertendo 
      //Temperatura Max é +50ºC
      //AD -> ºC = 1024 / (leitura * 50)
      // leitura*0.048
      temperaturaUsuario = (temperaturaUsuario*0.048)+28; //Temperatura minima é 28 então é de 28 a 77
      
      tempoEmCadaLeitura = 1; //Segundos
      //PID
      erro = temperaturaUsuario - temperaturaLida;
      
      //P -> incremento no valor de saida para ir corrigindo o erro
      p = erro * kp;
      
      //I -> soma o erro para fazer com que se aproxime a zero
      i += erro * ki * tempoEmCadaLeitura;
      
      //D -> variação da leitura passada para a leitura agora
      d = (ultimaTemperatura - temperaturaLida) * kd * tempoEmCadaLeitura;
      ultimaTemperatura = temperaturaLida;
      
      //Valor de negativo a positivo
      controlePID = p+i+d;
      
      //Saida Controle para o acionador;
      to = controlePID;
      
      printf(lcd_escreve,"\f E: %Ld T: %Ld \n",temperaturaUsuario,temperaturaLida);
      printf(lcd_escreve,"\r PID: %Ld \n",controlePID);
      fprintf(Serial,"%Ld %Ld %Ld %Ld\n",temperaturaUsuario,temperaturaLida,erro,controlePID);
      delay_ms(10);
   }
}
