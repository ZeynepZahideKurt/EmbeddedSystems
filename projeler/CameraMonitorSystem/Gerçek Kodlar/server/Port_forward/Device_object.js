//cihazların o an kullanılan wssi, videoya dönüştürlmemiş klasörünün olup olmadığı, klasör varsa hangi dakikanın kaydediliyor olduğu
//ve cihazın o anki durumunun canlı yayın mı video kaydı mı ya daa kayıt tamamlamladı mı belli olması sağlanıyor

class Device {
    constructor(mac) {
      this._mac = mac;
    }
    define_ws(ws) {
      this._ws = ws;
    }
    define_file(isim) {
      this._file_name = isim;
    }
    define_minutes(minutes) {
      this._file_minutes = minutes;
    }
    define_type(type) {
      this._type = type;
    }
    live_check(status){
        this._live=status;
    }
  }
  module.exports =Device;