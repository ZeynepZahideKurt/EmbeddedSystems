class Client {
    constructor(ws, mac, cookie, key) {
      this._mac = mac;
      this._ws = ws;
      this._cookie = cookie;
      this.key = key;
    }
    log_out(type) {
      this._avaible = type;
    }
  }
  module.exports = module.exports =Client;
