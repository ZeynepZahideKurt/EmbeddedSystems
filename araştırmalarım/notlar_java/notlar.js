

///quectel çalıştırmakla igili

if(1==1){
var not1="QUECTEL ÇALIŞTIRABİLMEK İÇİN POWERKEY UCUN 2 SANİYE 3.3 VOLT 2 SANİYE gnd verdim.";

}
///Tera Term'de UC200T Modülünü server ile HTTPGET Komutları

if(1==1){

    var not2="HTTP GET AT komutları(Tera Termde)";

    AT+QHTTPCFG="contextid",1   
    -OK

    AT+QHTTPCFG="responseheader",1  
    -OK

    AT+QIACT?
    -OK

    AT+QICSGP=1,1,"internet","gprs","gprs",1
    -OK

    AT+QIACT=1
    -OK
    
    AT+QIACT?
    - +QIACT: 1,1,1,"10.231.15.44"  
    -OK

    AT+QHTTPURL=24,80 
    -CONNECT

    http://89.19.23.98:8080/  
    -OK

    AT+QHTTPGET=80 
    -OK
    -QHTTPGET: 0,200,17";

    AT+QHTTPREAD=80 
    -OK
    }


//WEBSOCKET SEND AT komutları(Tera Termde)";
if(1==1){
    var not3="WEBSOCKET SEND AT komutları(Tera Termde);
    AT+QIACT=1
OK
AT+QICSGP=1,1,"internet","","",1
OK

AT+QIOPEN=1,0,"TCP","89.19.23.98",9966,0,0
OK

+QIOPEN: 0,0

+QIURC: "recv",0
AT+QISTATE=1,0
+QISTATE: 0,"TCP","89.19.23.98",9966,9437,2,1,0,0,"uart1"

OK
AT+QISEND=0
> zeynepten gelen veri
SEND OK
AT+QIRD=0,1500
+QIRD: 39
zehradan gelen verizeynepten gelen veri

OK

+QIURC: "closed",0

    
    }
//
if(1==1){
    var not3=".";

    
    }
//
if(1==1){
    var not3=".";

    
    }
//
if(1==1){
    var not3=".";

    
    }

