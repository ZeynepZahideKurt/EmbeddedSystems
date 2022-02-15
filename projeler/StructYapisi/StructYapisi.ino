//#include <stdio.h>

#define cihazsayisi 6

typedef struct oto{
  float motor;
  int renk;
  char model;
  int vites;
}oto_t;

void oto_yazdir(oto_t yoto);
void oto_renk_yazdir(int otorenk);
void oto_giris_al(oto_t *pGiris);


void oto_yazdir(oto_t yoto){
  printf("oto renk= %d");
  oto_renk_yazdir(yoto.renk);
  printf(" model= %d, vites= %d, motor=%f\r\n",yoto.model, 
                                               yoto.vites, 
                                               yoto.motor);
}
void oto_renk_yazdir(int otorenk){
  switch(otorenk){
    case 0:  printf("siyah\r\n"); break;
    case 1:  printf("kirmizi\r\n"); break;
    case 2:  printf("beyaz\r\n"); break;
    case 3:  printf("mavi\r\n"); break;
    default: printf("gecersiz renk kodu\r\n"); break;
  }
}
#define cihazsayisi 6

typedef struct cihaz_bilgileri{
  int sicaklik;
  int nem;
  int voltaj;
  int seri_numara;
}cihaz_t;

void setup() {
  // put your setup code here, to run once:
  //oto_t oto1={.renk=5, .model='A', .vites=5, .motor=1.6};
  oto_t  oto2,oto3;
  oto_t galeri[cihazsayisi];
  oto_t *poto;
  int i=0;
  for(i=0; i<cihazsayisi;i++){
    oto_giris_al(&galeri[i]);
  }
  for(i=0; i<cihazsayisi;i++){
    oto_yazdir(galeri[i]);
  }
  return;
//galeri[0].model= 'A';
//galeri[4].renk=1;
}

void loop() {
  // put your main code here, to run repeatedly:

}
