const express = require("express");
const cookieParser = require("cookie-parser");
const bodyParser = require("body-parser");
const WebSocket = require("ws");
var cors = require("cors");
const path = require("path");
var app = express();
const session = require("express-session");
const http = require("http").createServer(app);
const io = require("socket.io")(http, {
  cors: {
    allRoutes: true,
    origin: "*",
    methods: ["GET", "POST"],
  },
});
const fs = require("fs");
var dateFormat = require("dateformat");
const ffmpegPath = require("@ffmpeg-installer/ffmpeg").path;
const ffmpeg = require("fluent-ffmpeg");
ffmpeg.setFfmpegPath(ffmpegPath);
const ffprobePath = require("@ffprobe-installer/ffprobe").path;
ffmpeg.setFfprobePath(ffprobePath);
const { Buffer } = require("buffer");
var Promise = require("promise");
require("events").EventEmitter.prototype._maxListeners = 20;
const authTokens = {};
//var passport     = require('passport');
var router = express.Router();
const socketioJwt = require("socketio-jwt");
const crypto = require("crypto");
const { setInterval } = require("timers");
var nodemailer = require("nodemailer");
var giris = 0;
const Pool = require("pg").Pool;
var jwt = require("jsonwebtoken");
const CAMERA_PORT = 8888;
const DISPLAY_PORT = 8889;
const VideoClient = 8887;
const HTTP_PORT = 80;
const password = 1234;
const Device = require("./Device_Object");
const Client = require("./Client_object");
const databaseObject = require("./Database2");
http.listen(80, () => {
  console.log(`Http Server is listening at ${HTTP_PORT}`);
});
const cameraServer = new WebSocket.Server(
  { port: CAMERA_PORT, server: http },
  () => console.log(`Camera Server is listening at ${CAMERA_PORT}`)
);

const clientServer = new WebSocket.Server(
  { port: DISPLAY_PORT, server: http },
  () => console.log(`Client Server is listening at ${DISPLAY_PORT}`)
);
const VideoServer = new WebSocket.Server(
  { port: VideoClient, server: http },
  () => console.log(`Video Server is listening at ${VideoClient}`)
);

//client session o anda açık olan kullanıcıların maclerini, maillerini, cookie ve keylerini sakladığımız objemizdir
// eğer log out olursa cookie ve keyi silinerrek artık canlı yayın izlemsi engellenir
const Clientsession = {};
//o server ile bağlantı kurmuş bütün kameralardan oluşturulan device_object nesnelerini saklayan dizi
let deviceArray = [];
// şu an da canlı yayın isteği yapılmış bütün kameraları ve bu kamerayı izleyen clientleri tutuan dizi
let liveDeviceArray = {};

let clientObjectArray = [];
app.use(express.json());
app.use(bodyParser.urlencoded({ extended: true }));
//parametre olarak gelen klasörün içindeki fotoları yine kendisine parametre olarak gelen isimdeki klasöre yazan fonksiyon
const create_video = (klasör_adı, DeviceMac) => {
  console.log("video yapma kısmındayım ");

  return new Promise((resolve, reject) => {
    var ffmpegPath = require("@ffmpeg-installer/ffmpeg").path;
    var mkdirp = require("mkdirp");
    mkdirp("./V" + DeviceMac, function (err) {
      if (err) {
        console.error(err);
        resolve(-1);
      } else {
        ffmpeg.setFfmpegPath(ffmpegPath);
        ffmpeg()
          .input("./" + klasör_adı + "/test%d.jpg")
          .inputFPS(3)
          .videoCodec("libx264")
          .withOutputFormat("mp4")
          .outputOptions("-pix_fmt yuv420p")
          .save(
            "./V" +
              DeviceMac +
              "/G " +
              dateFormat(Date(), "yyyy-mm-dd h-MM-ss") +
              ".mp4"
          )

          .on("end", function () {
            console.log("downlaod resolved");
            resolve(true);
          })
          .on("error", function (err) {
            console.log(err);
            resolve(false);
          })
          .run();
      }
    });
  });
};
//video oluşturma fnsiyonunu  çağırıp olumlu cvabı aldığında videoya dönüştürülmüş klasörü siliyor
const start_creating_video = async (klasör_adı, DeviceIndex) => {
  console.log("video başlat kısmındayım ");
  return new Promise(async (resolve, reject) => {
    var video_status = await create_video(
      klasör_adı,
      deviceArray[DeviceIndex]._mac
    );
    if (video_status == true) {
      
      fs.rmSync("./" + klasör_adı, { recursive: true }, (err) => {
        if (err) return console.error(err);
        console.log("success!");
      });
    } else {
      console.log(
        klasör_adı +
          " bu klasör adındaki " +
          deviceArray[DeviceIndex]._mac +
          " mac adresine ait cihazın fotoğraf kayıtları videoya dönüştürülemedi"
      );
    }
    resolve(1);
  });
};
//kendisine gelen wsnin o an kullanılabilir cihazlardan birisine ait mi die kontrol edip eğer uyumlu ws bulunursa o cihazın ,cihazlar dizisindeki indisini geri döndürüyor
const ws_Query = (ws) => {
  return new Promise((resolve, reject) => {
    var kontrol = 0;
    for (var t = 0; t < deviceArray.length; t++) {
      if (ws == deviceArray[t]._ws) {
        kontrol = 1;
        resolve(t);
      }
    }
    if (kontrol == 0) {
      resolve(-1);
    }
  });
};
//parametre olarak gelen macin cihazlar listesindeki hangi cihaza ait olduğu bulunarak o cihazın indexini geri gönderirir
const mac_Query = (mac) => {
  return new Promise((resolve, reject) => {
    var kontrol = 0;
    for (var t = 0; t < deviceArray.length; t++) {
      if (mac == deviceArray[t]._mac) {
        kontrol = 1;
        resolve(t);
      }
    }
    if (kontrol == 0) {
      resolve(-1);
    }
  });
};
//parametrelerinden olan cihaz_index ile datası gelen cihazın kalsörünü buluyor yoksa oluşturuyor ve gelen datayı index ile belirtilen isimde bir fotoya dönüştürerek istenen kalsöre yazıyor
const create_photo = (data, DeviceIndex, photoCount) => {
  var buffer = new Buffer.from(data, "binary");

  /*
  
  console.log(buffer)
 var deneme=buffer.toString('hex');
 var deneme2=deneme.match(/.{1,2}/g);
var deneme3=deneme2.join(" ")

  fs.writeFile(
    "./" + "foto" + "/test" + photoCount + ".txt", deneme3,function (err, written) {
      if (err) console.log(err);
      else {
        //  console.log("Successfully written");
      }})*/
  var mkdirp = require("mkdirp");
  mkdirp("./" + deviceArray[DeviceIndex]._file_name, function (err) {
    if (err) console.error(err);
    else {
      fs.writeFile(
        "./" +
          deviceArray[DeviceIndex]._file_name +
          "/test" +
          photoCount +
          ".jpg",
        buffer,
        function (err, written) {
          if (err) console.log(err);
          else {
            //  console.log("Successfully written");
          }
        }
      );
    }
  });
};
//parametre olarak gelen verinin verinin veri tabanında kaydı var mı diye kontrol ediyor
const findMac = async (mac) => {
  const insertText = "SELECT id FROM  mac_table WHERE mac= $1 ";
  var response = await pool.query(insertText, [mac]);
  if (response.rowCount > 0) {
    //  console.log("mac buldum"+response.rows[0].id)
    return response.rows[0].id;
  } else {
    return -1;
  }
};
//cihazdan kayıt yapma isteği geldiğinde mailin veri tabanında kayılı ve mac adresinin önceden kaydının olmadığı durumlarda veri tabanına cihazın mac adresini kaydeder
const register_mac = async (mac, mail, ws) => {
  try {
    console.log("register");
    // önce qr koddan gelen mail kyaıtlı mı diye bakılyor
    findIdFromMail_response =await databaseObject.findIdFromMail(mail);

    if (findIdFromMail_response != -1) {
      // daha sonra bu cihaz veri tabınında kayıtlı mı diye bakılıyor
      findMac_response = await databaseObject.findMac(mac);
      // eğer kayıtlı değil ise veri tabanına ekleniyor
      if (findMac_response == -1) {
        const RegisterMac_insertText =
          "INSERT INTO mac_table( mac,f_id ) VALUES($1,$2) RETURNING id";

        const RegisterMac_response = await databaseObject.registerMac(
          mac,
          findIdFromMail_response,
        );
        //eğer başarılı olark eklendi ise o an açık olna sessionların içerisinde bu macın sahibi olan kullancı var mı diye bakılarak ar ise
        //mac cihazlarına yeni eklenen cihaz ekleniyor
        if (RegisterMac_response == true) {
          console.log("başarılı");
          ws.send("registerTrue");

          var cookieArray = Object.keys(Clientsession);

          cookieArray.forEach((cookie) => {
            if (Clientsession[cookie][1] == mail) {
              var MacArray = JSON.parse(Clientsession[cookie][2]);
              MacArray.push({ mac: mac });
              Clientsession[cookie][2] = JSON.stringify(MacArray);
            }
          });

          console.log(Clientsession);
          return 1;
        } else {
          console.log("başarısız");
          ws.send("registerFalse");
          return -1;
        }
      }
      // eğer veri tabınında kayıt bulunursa bu güncelleme işlemi yapılıyor
      else {
        const UpdateMac_insertText =
          "UPDATE mac_table SET F_id= $1 WHERE id=" + findMac_response + "";

        const UpdateMac_response = await databaseObject.updateMacUser(
          findMac_response,
          findIdFromMail_response,
        );
        //eğer güncelleme başarılı ise o zaman bu cihazı eski kullanan mailin sessionundan silinerek
        // yeni kayıt yaptırılan kullanıcının sessionuna ekleniyor
        if (UpdateMac_response.rowCount != 0) {
          var cookieArray = Object.keys(Clientsession);

          cookieArray.forEach((cookie) => {
            var MacArray = JSON.parse(Clientsession[cookie][2]);
            MacArray.forEach((element, i) => {
              if (element.mac == mac) {
                MacArray.splice(i, 1);
                Clientsession[cookie][2] = JSON.stringify(MacArray);
              }
            });
          });
          cookieArray.forEach((cookie) => {
            if (Clientsession[cookie][1] == mail) {
              var MacArray = JSON.parse(Clientsession[cookie][2]);
              MacArray.push({ mac: mac });
              Clientsession[cookie][2] = JSON.stringify(MacArray);
            }
          });
          console.log(Clientsession);
          console.log("update edildi");
          console.log("başarılı");
          ws.send("registerTrue");
          return 1;
        } else {
          console.log("update edilemedi");
          console.log("başarısız");
          ws.send("registerFalse");
          return -1;
        }
      }
    } else {
      console.log("başarısız");
      ws.send("registerFalse");
      return -1;
    }
  } catch (error) {
    console.log(error);
    console.log("başarısız");
    ws.send("registerFalse");
    return -1;
  }
};
//esp32yi dinlediğm kısım burası
cameraServer.on("connection", (ws, req) => {
  //console.log(ws)
  let kontrol = true;
  let intervallar = false;
  ws.on("error", function (e) {
    console.log("Client error: %s", e.message);
  });

  console.log("Camera connected");

  var photoCount = 0;
  let kop;
  let fal;
  ws.on("message", async (data) => {
    //   console.log("veri geldi")
    //console.log("geldim")
    //cihazların elektrik kesilmesi gibi durumlarında disconnect durumu oluşmuyor ama cihazla bağlantı kopuyor bu tür durumları anlamak için her cihaza bir interval atanır
    // ve 5 seniyede bir cihazı kontrol eder cevap alamsa koptu kabul eder
    if (intervallar == false) {
      kop = setInterval(() => {
        kontrol = false;
      }, 5000);
      fal = setInterval(async () => {
        if (kontrol == false) {
          //eğer cihazdan  5 saniye cevap alamazsa  cihazın wsinden indexini bulur ve o cihaza ait kaydedilmemiş bir fotoğraf kalsörü varsa onu videoya dönüştürür
          console.log("koptu ");
          DeviceNotAlive(ws);
          var DeviceIndex = await ws_Query(ws);
          if (DeviceIndex != -1) {
            try {
              //cihazdan bağlantı koptuğunda videoya dönüştürülmemiş bir fotoğraf klasörü var ise o videoya dönüştürülür
              if (deviceArray[DeviceIndex]._file_name != null) {
                start_creating_video(
                  deviceArray[DeviceIndex]._file_name,
                  DeviceIndex
                );
                deviceArray[DeviceIndex]._file_name = null;
              }
            } catch (error) {
              console.log(error);
            }
          }
          clearInterval(fal);
          clearInterval(kop);

          intervallar = false;
        }
      }, 10000);

      intervallar = true;
    }
    //console.log("geliyor");
    kontrol = true;
    //eğer cihazdan gelen data fotoğraf görüntüsü ise bu if çalışır
    if (data.length > 600) {
      //mesaj gelen wsnin hangi cihaza ait olduğunu bulmak için bir sorgulama yapılır ve cihaz bulunursa cihazın şuank tipine göre kaydetme ya da canlı yayın yapar
      var DeviceIndex = await ws_Query(ws);
      if (DeviceIndex != -1) {
        if (deviceArray[DeviceIndex]._type == "kaydet") {
          //tipi kaydet ise şuan bir kalsörü var mı diye bakılır var ise video yapılır yok ise
          if (deviceArray[DeviceIndex]._file_name == null) {
            photoCount = 0;
            //şuanki saniye 55den aşağı ise o dakika için yukarı ise bir sonraki dakika için  klasör oluşturulup foto oluşturulma yollanır
            if (dateFormat(Date(), "ss") >= 55) {
              deviceArray[DeviceIndex]._file_minutes =
                Number(dateFormat(Date(), "MM")) + 1;
              deviceArray[DeviceIndex]._file_name =
                "F;" +
                deviceArray[DeviceIndex]._mac +
                "--" +
                dateFormat(Date(), "h.MM.ss");
            } else {
              deviceArray[DeviceIndex]._file_minutes = dateFormat(Date(), "MM");
              deviceArray[DeviceIndex]._file_name =
                "F;" +
                deviceArray[DeviceIndex]._mac +
                "--" +
                dateFormat(Date(), "h.MM.ss");
            }
            console.log(
              "klasör oluştur " +
                deviceArray[DeviceIndex]._file_minutes +
                deviceArray[DeviceIndex]._file_name
            );

            create_photo(data, DeviceIndex, photoCount);
          } else if (
            dateFormat(Date(), "MM") > deviceArray[DeviceIndex]._file_minutes
          ) {
            //klasörün dakikası dolmuşsa klasör sıfırlanıp video yapılır
            console.log(
              "klasör sıfırla" +
                dateFormat(Date(), "MM ") +
                deviceArray[DeviceIndex]._file_name
            );
            start_creating_video(
              deviceArray[DeviceIndex]._file_name,
              DeviceIndex
            );
            deviceArray[DeviceIndex]._file_name = null;
          }
          // console.log(deviceArray[DeviceIndex]._mac);
          else if (deviceArray[DeviceIndex]._file_name != null) {
            console.log(
              "normal foto ekleme " +
                dateFormat(Date(), "MM ") +
                deviceArray[DeviceIndex]._file_name
            );
            photoCount++;
            create_photo(data, DeviceIndex, photoCount);
          }
        }

        liveBroadcast(ws, data);
        /*  connectedClients.forEach((ws, i) => {
          if (ws.readyState === ws.OPEN) {
            // console.log(data)
            ws.send(data);
          } else {
            connectedClients.splice(i, 1);
          }
        });*/
      }
    } else {
      //cihazdan görüntü gelmiyor ise  cihazın içerisindeki şifre doğru mu diye kontrol edilir
      console.log("şifreyi burada alabilirim");
      console.log(data);
      var json = JSON.parse(data);

      if (json.sifre == password) {
        var DeviceIndex = null;
        //kayıtlı bir cihazdan mı mesj geliyor diye kontrol edilir
        for (var t = 0; t < deviceArray.length; t++) {
          if (json.mac == deviceArray[t]._mac) {
            DeviceIndex = t;
            break;
          }
        }
        // kayıtlı bir cihazdan mesaj gelmiyorsa cihaz objesi oluşturularak diziye eklenir
        if (DeviceIndex == null) {
          if (json.type == "register") {
            var mailWitOutSpace = json.mail.replace(/\s/g, "");
            console.log(mailWitOutSpace);
            var sonuc = register_mac(json.mac, mailWitOutSpace, ws);
          } else {
            var newDevice = new Device(json.mac);
            newDevice.define_type(json.type);
            newDevice.define_ws(ws);
            newDevice.live_check(false);
            console.log(newDevice._live);
            deviceArray.push(newDevice);
          }
        } else {
          //cihaz kayıtlı bir cihaz ise  cihazın wssi ve tipi yeniden ayarlanır ve tipi kayme tamamlandı ise video kaydı yapılır
          deviceArray[DeviceIndex].define_ws(ws);
          deviceArray[DeviceIndex].live_check(false);
          deviceArray[DeviceIndex].define_type(json.type);
          if (json.type == "kaydettamamlandi") {
            try {
              if (deviceArray[DeviceIndex]._file_name != null) {
                start_creating_video(
                  deviceArray[DeviceIndex]._file_name,
                  DeviceIndex
                );
                deviceArray[DeviceIndex]._file_name = null;
              }
            } catch (error) {
              console.log(error);
            }
          }
          if (json.type == "register") {
            var mailWitOutSpace = json.mail.replace(/\s/g, "");
            console.log(mailWitOutSpace);
            var sonuc = register_mac(json.mac, mailWitOutSpace, ws);
          }

          console.log("burası" + deviceArray[DeviceIndex]._type);
        }
      }
    }
  });

  ws.on("close", async (data) => {
    clearInterval(kop);
    clearInterval(fal);
    var DeviceIndex = null;
    for (var t = 0; t < deviceArray.length; t++) {
      if (ws == deviceArray[t]._ws) {
        DeviceIndex = t;
        break;
      }
    }
    if (DeviceIndex == null) {
      console.log("cihaz hiç kaydedilmedi");
    } else {
      if (deviceArray[DeviceIndex]._file_name != null) {
        await start_creating_video(
          deviceArray[DeviceIndex]._file_name,
          DeviceIndex
        );
        deviceArray[DeviceIndex]._file_name = null;
      }
    }
    console.log("çıktıkii");
  });
});
// parametre olarka gelen macin shibi olna bir client var mı diye kontrol ederek uyumlu client bulunursa bu clientı bir diziye atıyor ve en sonunda clientObjectArray dizisini geri dönüyor
const clientQuery = (mac) => {
  return new Promise((resolve, reject) => {
    var kontrol = 0;
    var clientArray = [];
    for (var t = 0; t < clientObjectArray.length; t++) {
      clientObjectArray[t]._mac.forEach(async (index) => {
        if (mac == index) {
          clientArray.push(t);
          kontrol = 1;
          //resolve(t);
        }
      });
    }
    if (kontrol == 0) {
      resolve(-1);
    } else {
      resolve(clientArray);
    }
  });
};
//parametre olarak gelen wsnin önce cihazını buluyor daha sonra o cihazın clientlarını buluyor ve o clientlara görüntü ve cihazın adını içeren bir json döndürüyor
const liveBroadcast = async (ws, data) => {
  var deviceIndex = await ws_Query(ws);

  if (deviceIndex != -1) {
    var clientArray = await clientQuery(deviceArray[deviceIndex]._mac);

    if (clientArray != -1) {
      clientArray.forEach((clientIndex) => {
        if (
          clientObjectArray[clientIndex]._ws.readyState === ws.OPEN &&
          clientObjectArray[clientIndex]._avaible == false
        ) {
          let json = {
            mac: deviceArray[deviceIndex]._mac,
            blob: data,
            dataLength: data.length,
          };
          if (deviceArray[deviceIndex]._live == false) {
            console.log(" bu cihazda ilk kez canlı yayın başladı");
            deviceArray[deviceIndex].live_check(true);
          }

          clientObjectArray[clientIndex]._ws.send(JSON.stringify(json));
        }
      });
    }
  }
};
//Eğer cihazla olan bağlantı koparsa cihazın canlı olma durumunu clientObjectArray dizinde false olarak günceller
const DeviceNotAlive = async (ws) => {
  var deviceIndex = await ws_Query(ws);

  if (deviceIndex != -1) {
    var clientArray = await clientQuery(deviceArray[deviceIndex]._mac);

    if (clientArray != -1) {
      clientArray.forEach((clientIndex) => {
        if (
          clientObjectArray[clientIndex]._ws.readyState === ws.OPEN &&
          clientObjectArray[clientIndex]._avaible == false
        ) {
          clientObjectArray[clientIndex]._ws.send("deviceDisconnect");
        }
      });
    }
  }
};
//paramet olarak gelen datayı jsona parse eder
const creteJSonObject = async (data) => {
  return new Promise((resolve) => {
    resolve((data = JSON.parse(data)));
  });
};
//canlı yayını kapataan client ilgili diziden silinir
const DeleteObjectFromClientObjectArray = async (ws) => {
  return new Promise((resolve) => {
    for (var t = 0; t < clientObjectArray.length; t++) {
      clientObjectArray.forEach(async (client) => {
        if (ws == clientObjectArray[t]._ws) {
          clientObjectArray.splice(t, 1);
          resolve(1);
        }
      });
    }
  });
};
//eğer canlı yayın isteğ yollanılan cihazlar dizisindeki cihazlara ilk seferde bağlan mesajı yollanılamadı ise 5 saniyede bir bağlan mesajı yollanır
var deviceConnect = setInterval(async () => {
  var LİVEdeviceArray = Object.keys(liveDeviceArray);
  LİVEdeviceArray.forEach(async (mac) => {
    var DeviceIndex = await mac_Query(mac);

    if (DeviceIndex != -1) {
      if (deviceArray[DeviceIndex]._live == false) {
        console.log("canlı yayın aç mesajı verildi");
        deviceArray[DeviceIndex]._ws.send("bağlan");
      }
    }
  });
}, 5000);
deviceConnect;
//client soketinin çalışmasını kontrol eder
clientServer.on("connection", (ws, req) => {
  ws.on("error", function (e) {
    console.log("Client error: %s", e.message);
  });
  ws.on("close", function (e) {
    console.log("Client çıktı");

    var LİVEdeviceArray = Object.keys(liveDeviceArray);
    LİVEdeviceArray.forEach((mac) => {
      liveDeviceArray[mac].forEach(async (client, index) => {
        if (client.ws == ws) {
          var DeviceIndex = await mac_Query(mac);
          if (DeviceIndex != -1) {
            if (liveDeviceArray[mac].length == 1) {
              deviceArray[DeviceIndex].live_check(false);
              deviceArray[DeviceIndex]._ws.send("kop");
              delete liveDeviceArray[mac];
              console.log(liveDeviceArray);
            } else {
              liveDeviceArray[mac].splice(index, 1);
              console.log(liveDeviceArray);
            }
          } else {
            if (liveDeviceArray[mac].length == 1) {
              delete liveDeviceArray[mac];
              console.log(liveDeviceArray);
            } else {
              liveDeviceArray[mac].splice(index, 1);
              console.log(liveDeviceArray);
            }
          }
        }
      });
    });
    DeleteObjectFromClientObjectArray(ws);
  });

  //clienttan aldığı coookie ve key değerlerini olan sessionu bulunann clientObjectArray arasıdna arıyor uyumlu bulursa çalışmaya başlıyor
  ws.onmessage = async (message) => {
    let clientMessage = await creteJSonObject(message.data);
    // console.log(message)
    if (
      Clientsession.hasOwnProperty(clientMessage[1].cookie) == true &&
      Clientsession[clientMessage[1].cookie][0] == clientMessage[1].key
    ) {
      if (clientMessage[0].message == "bağlan") {
        console.log("bağlanıyorum");
        var MacJson = JSON.parse(Clientsession[clientMessage[1].cookie][2]);
        //bağlan mesajı web sitesinden geliyorsa try kısmı telefondan geliyorsa catch kısm çağılır
        //eğer masaj webden geliyorsa gelen mesajdaki macler session yapısında tuttuğumuz maclerle karşılatırılarak uyumlu olanlara bağlan mesajı yollanır
        //telefonda ise bütün cihazlar anı anda bağlandığı için direk session yapısındaki maclere bağlan mesajı yollanır
        let macArray = [];
        try {
          clientMessage[2].forEach((obj) => {
            MacJson.map(async (object) => {
              if (object.mac == obj.isim) {
                macArray.push(obj.isim);
                if (liveDeviceArray.hasOwnProperty(obj.isim)) {
                  liveDeviceArray[obj.isim].push({ ws: ws });
                } else {
                  liveDeviceArray[obj.isim] = [{ ws: ws }];
                  var DeviceIndex = await mac_Query(obj.isim);

                  if (DeviceIndex != -1) {
                    if (deviceArray[DeviceIndex]._live == false) {
                      console.log("canlı yayın aç mesajı verildi");
                      deviceArray[DeviceIndex]._ws.send("bağlan");
                    }
                  } else {
                    console.log("cihaz kapalı");
                  }
                }
              }
            });
          });
        } catch (error) {
          console.log(error);
          MacJson.map(async (obj, i) => {
            macArray.push(obj.mac);
            if (liveDeviceArray.hasOwnProperty(obj.mac)) {
              liveDeviceArray[obj.mac].push({ ws: ws });
            } else {
              liveDeviceArray[obj.mac] = [{ ws: ws }];
              var DeviceIndex = await mac_Query(obj.isim);

              if (DeviceIndex != -1) {
                if (deviceArray[DeviceIndex]._live == false) {
                  console.log("canlı yayın aç mesajı verildi");
                  deviceArray[DeviceIndex]._ws.send("bağlan");
                }
              } else {
                console.log("cihaz kapalı");
              }
            }
          });
        }
        console.log(liveDeviceArray);
        // eğer clienttan bağlan mesajını alırsa clientın wsini, mac adreslerini ,cookiesini ve keyini bir kişi objesine atarak o kişi obejesini clientObjectArray dizisine atar
        var newClient = new Client(
          ws,
          macArray,
          clientMessage[1].cookie,
          clientMessage[1].key
        );
        newClient.log_out(false);
        var clientCheck = 0;
        for (var y = 0; y < clientObjectArray.length; y++) {
          if (ws == clientObjectArray[y]._ws) {
            clientCheck = y;
          }
        }
        if (clientCheck == 0) {
          clientObjectArray.push(newClient);
        } else {
          clientObjectArray[clientCheck] = newClient;
        }

        /*  //o anki clinetımızın maclari kullanılarak cihazlar listesindeki indexi ve wsi bulunarak o cihaza bağlan mesajı yollanıyor
         newClient._mac.forEach(async (mac) => {
           var DeviceIndex = await mac_Query(mac);
 
           if (DeviceIndex != -1) {
             if (deviceArray[DeviceIndex]._live == false) {
               console.log("canlı yayın aç mesajı verildi");
               deviceArray[DeviceIndex]._ws.send("bağlan");
             }
           } else {
             console.log("cihaz kapalı");
           }
         });
         //eğer cihaza ilk seferde bağlan mesajı yollanılamadı ise 5 saniyede bir bağlan mesajı yollanır
         var deviceConnect = setInterval(async () => {
           if (clientObjectArray.includes(newClient) == true) {
             newClient._mac.forEach(async (mac) => {
               var DeviceIndex = await mac_Query(mac);
 
               if (DeviceIndex != -1) {
                 if (deviceArray[DeviceIndex]._live == false) {
                   console.log("canlı yayın aç mesajı verildi");
                   deviceArray[DeviceIndex]._ws.send("bağlan");
                 }
               }
             });
           } else {
             clearInterval(deviceConnect);
           }
         }, 5000);*/
      } else if (clientMessage[0].message == "kop") {
        /*for (var i = 0; i < connectedClients.length; i++) {
          if (ws == connectedClients[i]) {
            connectedClients.splice(i, 1);
          }
        }*/
        //client kopma isteği yolladığında  o clientın her maci içi teker teker cihaz bulunur ve o cihazlara kop mesajjı yollanır
        // o client ,clientObjectArray dizisinden silinir ve live device dizinden cihazlar kaldırılır
        var LİVEdeviceArray = Object.keys(liveDeviceArray);
        LİVEdeviceArray.forEach((mac) => {
          liveDeviceArray[mac].forEach(async (client, index) => {
            if (client.ws == ws) {
              var DeviceIndex = await mac_Query(mac);

              if (DeviceIndex != -1) {
                if (liveDeviceArray[mac].length == 1) {
                  deviceArray[DeviceIndex].live_check(false);
                  deviceArray[DeviceIndex]._ws.send("kop");
                  delete liveDeviceArray[mac];
                  console.log(liveDeviceArray);
                } else {
                  liveDeviceArray[mac].splice(index, 1);
                  console.log(liveDeviceArray);
                }
              } else {
                if (liveDeviceArray[mac].length == 1) {
                  delete liveDeviceArray[mac];
                  console.log(liveDeviceArray);
                } else {
                  liveDeviceArray[mac].splice(index, 1);
                  console.log(liveDeviceArray);
                }
              }
            }
          });
        });
        DeleteObjectFromClientObjectArray(ws);

        //client kopma isteği yolladığında  o clientın her maci içi teker teker cihaz bulunur ve o cihazlara kop mesajjı yollanır ve o client ,clientObjectArray dizisinden silinir
        /*  for (var t = 0; t < clientObjectArray.length; t++) {
  
            clientObjectArray[t]._mac.forEach(async (mac) => {
  
              if (clientMessage[2].isim == mac) {               
                console.log("buradayım")
  
                clientObjectArray[t]._mac.forEach(async (mac) => {
                  var DeviceIndex = await mac_Query(mac);
                  if (DeviceIndex != -1) {
                    deviceArray[DeviceIndex].live_check(false);
                    deviceArray[DeviceIndex]._ws.send("kop");
                  }
                });
                clientObjectArray.splice(t, 1);
              }
            });
            console.log(clientObjectArray)
          }*/
      }
    } else {
      // eğer clientın cookie ve keyi o an sessionu bulunan bir clieanta ait değilse bu kısım çalışır ve log out mesajı yollanarak arayüzün login sayfasına dönmesi sağlanır
      ws.send("log_out");
      //o clientın her maci içi teker teker cihaz bulunur ve o cihazlara kop mesajjı yollanır ve o client ,clientObjectArray dizisinden silinir
      var LİVEdeviceArray = Object.keys(liveDeviceArray);
      LİVEdeviceArray.forEach((mac) => {
        liveDeviceArray[mac].forEach(async (client, index) => {
          if (client.ws == ws) {
            var DeviceIndex = await mac_Query(mac);

            if (DeviceIndex != -1) {
              if (liveDeviceArray[mac].length == 1) {
                deviceArray[DeviceIndex].live_check(false);
                deviceArray[DeviceIndex]._ws.send("kop");
                delete liveDeviceArray[mac];
                console.log(liveDeviceArray);
              } else {
                liveDeviceArray[mac].splice(index, 1);
                console.log(liveDeviceArray);
              }
            } else {
              if (liveDeviceArray[mac].length == 1) {
                delete liveDeviceArray[mac];
                console.log(liveDeviceArray);
              } else {
                liveDeviceArray[mac].splice(index, 1);
                console.log(liveDeviceArray);
              }
            }
          }
        });
      });
      DeleteObjectFromClientObjectArray(ws);
    }
  };
});
//video kaydı izlene kısımdaki web soket bağlantısının tamamı bu fonsiyon ile yönetilir
VideoServer.on("connection", (ws, req) => {
  let DeviceMac;
  let DeviceFolder;
  let veriler = [];
  let video_list_response = {};
  let t = 0;
  console.log("welcome to video server ");

  ws.on("error", function (e) {
    console.log("Client error: %s", e.message);
  });
  ws.on("close", function (e) {
    console.log(" Video Client çıktı");
  });
  ws.on("message", async (data) => {
    data = JSON.parse(data);

    try {
      //gelen isteğin sessionu olup olmadığı kontrol edilir
      if (
        Clientsession.hasOwnProperty(data.cookie) == true &&
        Clientsession[data.cookie][0] == data.key
      ) {
        let MacJson = JSON.parse(Clientsession[data.cookie][2]);
        if (data.mac == undefined) {
        } else {
          MacJson.forEach((element) => {
            if (element.mac == data.mac) {
              DeviceMac = data.mac;
              DeviceFolder = "./V" + DeviceMac + "/";
              // console.log(DeviceMac)
            }
          });
        }
        switch (data.type) {
          case "DefineMacFileName":
            try {
              fs.readdir("./V" + data.mac + "/", (err, files) => {
                //console.log(files)

                if (files != undefined) {
                  ws.send(
                    JSON.stringify({
                      type: "video_list_response",
                      VideoList: files,
                    }).toString()
                  );
                } else {
                  ws.send(
                    JSON.stringify({
                      type: "error_message",
                      message: "Seçilen cihazın video kaydı bulunmamaktadır.",
                    }).toString()
                  );
                }
              });
            } catch (error) {
              ws.send(
                JSON.stringify({
                  type: "error_message",
                  message: "Seçilen cihazın video kaydı bulunmamaktadır.",
                }).toString()
              );
            }
            break;
          case "show_video":
            ws.send(
              JSON.stringify({
                type: "videoUrl",
                url:
                  "?cookie=" +
                  data.cookie +
                  "&key=" +
                  data.key +
                  "&device=" +
                  DeviceMac +
                  "&adi=./V" +
                  DeviceMac +
                  "/G " +
                  data.video +
                  ".mp4",
              }).toString()
            );
            // console.log(data.video);
            break;
        }
      } else {
        ws.send(JSON.stringify({ type: "log_out" }.toString()));
      }
    } catch (error) {
      console.log(error);
      ws.send(JSON.stringify({ type: "log_out" }.toString()));
    }
  });
});
app.use(cookieParser());

app.use(cors());
//gelen isteğin cookie ve keyini kontrol eder  ve doğru ise istenen videoyu yükler
app.get("/video", async (req, res) => {
  console.log("BANA GELDİ");
  if (req.query.adi == undefined) {
    res.writeHead(404, { "Content-Type": "text/html" });
    res.write("<h2>404 Not Found\n</h2>");
    res.end();
  } else {
    try {
      var clientCheck = false;
      var macJson = await creteJSonObject(Clientsession[req.query.cookie][2]);

      macJson.forEach((element) => {
        if (element.mac == req.query.device) {
          clientCheck = true;
        }
      });
      if (
        Clientsession.hasOwnProperty(req.query.cookie) == true &&
        Clientsession[req.query.cookie][0] == req.query.key &&
        clientCheck == true
      ) {
        const path = "" + req.query.adi + "";

        fs.access(path, fs.F_OK, (err) => {
          if (err) {
            res.writeHead(404, { "Content-Type": "text/html" });
            res.write("<h2>404 Not Found\n</h2>");
            res.end();
            return;
          }
          var stat = fs.statSync(path);
          var total = stat.size;

          if (req.headers["range"]) {
            var range = req.headers.range;
            var parts = range.replace(/bytes=/, "").split("-");
            var partialstart = parts[0];
            var partialend = parts[1];

            var start = parseInt(partialstart, 10);
            var end = partialend ? parseInt(partialend, 10) : total - 1;
            var chunksize = end - start + 1;
            // console.log("RANGE: " + start + " - " + end + " = " + chunksize);

            var file = fs.createReadStream(path, { start: start, end: end });
            res.writeHead(206, {
              "Content-Range": "bytes " + start + "-" + end + "/" + total,
              "Accept-Ranges": "bytes",
              "Content-Length": chunksize,
              "Content-Type": "video/mp4",
            });
            file.pipe(res);
          } else {
            console.log("ALL: " + total);

            res.writeHead(200, {
              "Content-Length": total,
              "Content-Type": "video/mp4",
            });

            fs.createReadStream(path).pipe(res);
          }
        });
      }
    } catch (error) {
      console.log("izinsiz");
    }
  }
});
//şifrenin için ilk şifrelemenin yapıldığı fonksiyon
async function hash(password) {
  return new Promise((resolve, reject) => {
    // generate random 16 bytes long salt
    const salt = crypto.randomBytes(16).toString("hex");

    crypto.scrypt(password, salt, 64, (err, derivedKey) => {
      if (err) reject(err);

      resolve(derivedKey.toString("hex"));
    });
  });
}
// ana şifreleme fonksiyonu
async function hash2(password, salt) {
  return new Promise(async (resolve, reject) => {
    // generate random 16 bytes long salt
    const ilk_şifre = await hash(salt);

    crypto.scrypt(password, ilk_şifre, 64, (err, derivedKey) => {
      if (err) reject(err);

      resolve(derivedKey.toString("hex"));
    });
  });
}
//login şifre değiştirme ve log_out işlemleri için çalışır
app.post("/api", async (req, res) => {
  try {
    const { type } = req.body;
    if (type == "login") {
      console.log("login isteği");
      //login isteği geldiğinde mail ve şifresi ile arama yapar ve bir eşleşme bulursa cookie ve key oluşturur o kişi için ve o id ile macler tablosunda arama yapar
      //bulunan macleri ve coookie clienta yollar ve kendisi de o kişiyi session dizine atar ve süresi dolduğunda da o listeden siler
      const { mail, sifre, device } = req.body;
      try {
        const logIn_response = await databaseObject.login(
          mail,
          crypto
            .pbkdf2Sync(sifre, "enelsis", 1000, 64, `sha512`)
            .toString(`hex`)
        );

        if (logIn_response != -1) {
          const mail_crypto = await hash2(
            mail,
            "enelsisnmdfbf2c6fs8dbdfbdx8plfold2jfbdjnbj8286vknvkdjfgn"
          );

          const cokkie_crypto = await hash2(
            mail_crypto,
            "jgdfögct45u9t98ekxcmsdı34tu8övgec930ktgnsoosıut83trngn"
          );

          Clientsession[cokkie_crypto] = [
            mail_crypto,
            mail,
            JSON.stringify(logIn_response),
          ];
          res.json({
            message: "kullanıcı girişi başarılı",
            cookie: cokkie_crypto,
            key: mail_crypto,
            mac: logIn_response,
            status: true,
          });
        } else {
          res.json({
            message: "Kullanıcı Adı Veya Şifre Yanlış",
            cookie: null,
            key: null,
            mac: null,
            status: false,
          });
        }
      } catch (error) {
        res.json({
          message: "Kullanıcı Adı Veya Şifre Yanlış",
          cookie: null,
          key: null,
          mac: null,
          status: false,
        });
        console.log(error);
      }

      // console.log(req.session.ad)
      // delete myObject['key02']
    } else if (type == "log_out") {
      //kulanıcıyı session listesinden siler
      console.log("log_out");
      try {
        const { cookie } = req.body;
        delete Clientsession[cookie];

        res.json({ result: true });
      } catch (error) {
        res.json({ result: false });
      }
    } else if (type == "changePassword") {
      console.log("changePassword");

      try {
        const { newPassword, oldPassword, cookie, key } = req.body;

        if (
          Clientsession.hasOwnProperty(cookie) == true &&
          Clientsession[cookie][0] == key
        ) {
          await databaseObject.changePassword(
            Clientsession[cookie][1],
            crypto
              .pbkdf2Sync(newPassword, "enelsis", 1000, 64, `sha512`)
              .toString(`hex`),
            crypto
              .pbkdf2Sync(oldPassword, "enelsis", 1000, 64, `sha512`)
              .toString(`hex`),
            res
          );
        } else {
          res.json({
            success: false,
            message: "Tekrar Giriş Yapınız",
          });
          console.log("hatalı");
        }
      } catch (error) {
        console.log(error);
      }
    } else if (type == "singIn") {
      try {
        const { mail, sifre } = req.body;
        console.log(mail);

        const response = await databaseObject.findIdFromMail(mail);
        if (response != -1) {
          res.json({ message: "Bu mail ile kayıt bulundu", status: false });
        } else {
          var verify_token = crypto
            .pbkdf2Sync(
              mail + sifre,
              "enelsis_verify_token",
              1000,
              64,
              `sha512`
            )
            .toString(`hex`);
          var smtpTransport = nodemailer.createTransport({
            service: "gmail",
            auth: {
              user: "verifydeneme.enelsis@gmail.com",
              pass: "enelsis2382985",
            },
          });

          link =
            "http://192.168.1.239/verify" +
            "/" +
            mail +
            "/verify?id=" +
            verify_token;
          mailOptions = {
            to: mail,
            subject:
              " Enelsis Güvenlik Kamerası Sistemi Kullanıcı Aktivasyon Mesajı",
            html:
              "Merhaba Değerli Yeni Kullanıcımız,<br>Enelsis Güvenlik Kamerası Sistemine giriş yapabilmek için aşağıdaki bağlantıyı tıklayarak hesabınızı doğrulamalısınız. <br>Emailinizi doğrulamak için lütfen linki tıklayınız.<br><a href=" +
              link +
              ">Doğrulama İçin Tıklayınız</a> <br> Bu e-posta Enelsis Güvenlik Kamerası Sistemi Kimlik Doğrulama Servisi tarafından otomatik olarak oluşturulmuştur. Eğer sisteme kayıt olma isteği sizin tarafınızdan yapılması ise bu e-postayı görmezden geliniz ",
          };
          smtpTransport.sendMail(mailOptions, async function (error, response) {
            if (error) {
              res.json({ message: "Teknik Bir Hata Oluştu ", status: false });
            } else {
              await databaseObject.singIn(
                mail,
                crypto
                  .pbkdf2Sync(sifre, "enelsis", 1000, 64, `sha512`)
                  .toString(`hex`),
                verify_token,
                res
              );
            }
          });
        }
      } catch (error) {
        console.log(error);
        res.json({ message: "Teknik Bir Hata Oluştu", status: false });
      }
    }
  } catch (error) {
    console.log(error);
    res.json({ message: "Teknik Bir Hata Oluştu", status: false });
  }
});
//arayüzden gelen session kontorl işlemlerini kontrol eder uyumlu sesion bulunursa clienta gerekli bilgileri geri yollar
app.post("/session", async (req, res) => {
  try {
    console.log("session_kontrol");

    try {
      const { cookie, device } = req.body;
      if (Clientsession.hasOwnProperty(cookie) == true) {
        if (device == "phone") {
          res.json({
            result: true,
            cookie: cookie,
            key: Clientsession[cookie][0],
            macler: JSON.parse(Clientsession[cookie][2]),
          });
        } else {
          res.json({
            result: true,
            cookie: cookie,
            key: Clientsession[cookie][0],
            macler: Clientsession[cookie][2],
          });
        }
      } else {
        res.json({ result: false });
      }
    } catch (error) {
      console.log(error);
      res.json({ result: false });
    }
  } catch (error) {
    console.log(error);
    res.json({ result: false });
  }
});
app.get("/verify/**/", async (req, res) => {
  var url = req.url.toString();
  var url_dizi = url.split("/");

  try {
    await databaseObject.verifyUser(url_dizi[2], req.query.id, res);
  } catch (error) {
    res.writeHead(200, { "Content-Type": "text/html; charset=UTF-8" });
    res.write("<h2>Emailiniz Doğrulanamadı\n</h2>");
    res.end();
  }
});

app.use(function (req, res, next) {
  res.writeHead(404, { "Content-Type": "text/html" });
  res.write("<h2>404 Not Found\n</h2>");
  res.end();
});
/*io.of("/io").on("connection", async (soket) => {
  //io soketinin işlerini kontrol etmek için yazılmıştır
  console.log(soket.id)
  soket.on("io", (gelen) => {
    console.log("io istek geldi");

    let key
    try {
      key = JSON.parse(gelen);
    } catch (error) {
      key = gelen;

    }

    try {

      if (
        Clientsession.hasOwnProperty(key.cookie) == true &&
        Clientsession[key.cookie][0] == key.key
      ) {
        console.log("io geldi ");
        var DeviceMac;
        let DeviceFolder;
        let veriler = [];
        let video_list_response = {};
        let t = 0;
        soket.on("set_mac_file", (data) => {



          try {
            if (
              Clientsession.hasOwnProperty(data.cookie) == true &&
              Clientsession[data.cookie][0] == data.key
            ) {
              DeviceMac = data.mac;

              DeviceFolder = "./V" + DeviceMac + "/";
            } else {

              soket.emit("log_out");
            }

          } catch (error) {

            soket.emit("log_out");
          }
        });
        soket.on("video_list_request", (data) => {
          try {
            if (
              Clientsession.hasOwnProperty(data.cookie) == true &&
              Clientsession[data.cookie][0] == data.key
            ) {
              console.log("geldi")

              fs.readdir(DeviceFolder, (err, files) => {
                VideoList = [];
                try {
                  files.forEach((file) => {
                    let json = { "isim": "" + file.substring(2, file.length - 4) + "" };

                    VideoList.push(json);
                  });
                  console.log(VideoList)
                  soket.emit("video_list_response", VideoList);
                } catch (error) {

                  soket.emit("error_message", { "message": "Seçilen cihazın video kaydı bulunmamaktadır." });
                }
              });
            } else {
              console.log("emit ettim");
              soket.emit("log_out");
            }

          } catch (error) {
            console.log("emit ettim");
            soket.emit("log_out");


          }
        });

        soket.on("show_video", (data) => {
          //cookie ile keyi kontrol etmeyi tamamlaman gerekiyor

          try {
            if (
              Clientsession.hasOwnProperty(data.cookie) == true &&
              Clientsession[data.cookie][0] == data.key
            ) {
              soket.emit("video", {
                "url":
                  "?cookie=" +
                  data.cookie +
                  "&key=" +
                  data.key + "&device=" + DeviceMac +
                  "&adi=./V" +
                  DeviceMac +
                  "/G " + data.video + ".mp4",
              });
              console.log(data);
            } else {
              soket.emit("log_out");
            }
          } catch (error) {
            soket.emit("log_out");
          }
        });


        soket.on("reset", () => {
          console.log("resetlendi");
          for (var t = 0; t < deviceArray.length; t++) {
            if (DeviceMac == deviceArray[t]._mac) {
              deviceArray[t]._ws.send("kamera resetle");
              start_creating_video(t);
            }
          }

        });
      } else {
        soket.emit("log_out");

      }
    } catch (error) {
      soket.emit("log_out");
    }
  });

  soket.on("disconnect", () => {
    console.log("io çıktı");
  });
  soket.on("error", function (e) {
    console.log("Client error: %s", e.message);
  });
});
io.of("/io").on("connection", (soket) => {
  //console.log(soket.handshake)
  soket.handshake.headers.authorization = "bplkfmkbmfd";

  var DeviceMac;
  let DeviceFolder;
  let veriler = [];
  let t = 0;
  soket.on("visible", (data) => {
    console.log("io'dayım");
    DeviceMac = data;
    DeviceFolder = "./V" + DeviceMac + "/";
  });
  soket.on("yolla", () => {
    veriler = [];
    fs.readdir(DeviceFolder, (err, files) => {
      try {
        files.forEach((file) => {
          let json = { isim: "" + file + "" };
          veriler.push(json);
        });

        console.log(veriler.length);
        soket.emit("liste", JSON.stringify(veriler));
      } catch (error) {
        console.log(error);
      }
    });
  });

  soket.on("tiklandi", (data) => {
    soket.emit("deneme", { sonuc: "?adi=./V" + DeviceMac + "/" + data });
    console.log(data);
  });

  soket.on("reset", () => {
    console.log("resetlendi");
    for (var t = 0; t < deviceArray.length; t++) {
      if (DeviceMac == deviceArray[t]._mac) {
        cihazlar[t]._ws.send("kamera resetle");
        start_creating_video(t);
      }
    }
  });

  soket.on("disconnect", () => {
    console.log("io çıktı");
  });
  soket.on("error", function (e) {
    console.log("Client error: %s", e.message);
  });
});


 jwt kullanmaya karar verilirse kullanıcalk olan kısım
    const { username, password } = req.body;
    const insertText = "SELECT id FROM  login WHERE mail= $1 AND sifre = $2";
    const sonuc = await pool.query(insertText, [username, password]);
    if (sonuc.rowCount == 1) {
      const insertText2 = "SELECT mac FROM  mac_table WHERE f_id= $1  ";
      const sonuc2 = await pool.query(insertText2, [sonuc.rows[0].id]);
      const token = jwt.sign({

        memail: username,
        mac:JSON.stringify(sonuc2.rows)
    },
    'secret_key',
    {
        expiresIn :"1d"
    }
    )
     //  console.log(sonuc2)
     return res.status(200).send({ message: 'success',mac:JSON.stringify(sonuc2.rows), token: token});
    }
    //
const generateAuthToken = () => {
  return crypto.randomBytes(30).toString("hex");
};
app.use((req, res, next) => {
  const authToken = req.cookies["AuthToken"];

  req.user = authTokens[authToken];

  next();
});
const requireAuth = (req, res, next) => {
  if (req.user) {
    next();
  } else {
    res.redirect("/login");
  }
};
//login soketi
soket.on("login", () => {
    usserSession[soket.id] = soket;
    console.log("login geldi");
    soket.on("yönlendir", async (data) => {
      try {
        const pool = new Pool({
          user: "postgres",
          password: 2382985,
          host: "localhost",
          port: 5432,
          database: "Kamera",
        });
        let sonuc1 = await creteJSonObject(data);
        //sifre = "+sifre +"::varchar
        const mail = sonuc1.mail;
        const sifre = sonuc1.sifre;
        const insertText =
          "SELECT id FROM  login WHERE mail= $1 AND sifre = $2";
        const sonuc = await pool.query(insertText, [mail, sifre]);

        if (sonuc.rowCount == 1) {
          const insertText2 = "SELECT mac FROM  mac_table WHERE f_id= $1  ";
          const sonuc2 = await pool.query(insertText2, [sonuc.rows[0].id]);
          console.log(sonuc2.rows);
          soket.handshake.session.userdata = mail;
          soket.handshake.session.save();
          //console.log(soket.handshake)
          soket.emit("mac_al", JSON.stringify(sonuc2.rows));
        } else {
          soket.emit("mac_al", -1);
        }
      } catch (error) {
        console.log(error);
      }
    });
  });
//








const server = http.createServer((req, res) => {
  res.writeHead(200, { "Content-Type": "text/html" });

  fs.readFile("./io_deneme.html", null, async function (error, data) {
    if (error) {
      res.writeHead(404);
      res.write("file not found");
    } else {
      res.write(data);
    }
    res.end();
  });
});
io.on("connection", (soket) => {

  var DeviceMac = "24-0A-C4-60-69-4C";
  const DeviceFolder = "./V" + DeviceMac + "/";
  var veriler = [];
  fs.readdir(DeviceFolder, (err, files) => {
    files.forEach((file) => {
      veriler.push(file);
    });
    soket.emit("liste", veriler);
  });

  soket.on("tiklandi", (data) => {
    soket.emit("deneme", { sonuc: "?adi=./V" + DeviceMac + "/" + data });
    console.log(data);
  });

  soket.on("reset", () => {
    console.log("resetlendi");
    for (var t = 0; t < cihazlar.length; t++) {
      if (DeviceMac == cihazlar[t]._mac) {
        cihazlar[t]._ws.send("kamera resetle");
        start_creating_video(t);
      }
    }
  });

  soket.on("disconnect", () => {
    console.log("çıktı");
  });
});*/
