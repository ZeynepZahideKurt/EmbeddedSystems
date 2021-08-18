const express = require("express");
const cookieParser = require('cookie-parser');
const bodyParser = require('body-parser');
const WebSocket = require("ws");
const path = require("path");
var app = express();
const http = require("http").createServer(app)
const io = require("socket.io")(http);
const cors = require("cors");
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
var router       = express.Router();
const socketioJwt = require('socketio-jwt');
const crypto = require('crypto');
var giris = 0;
app.use(cors());
app.use(express.json());
const users = [
  // This user is added to the array to avoid creating new user on each restart
  {
      
      email: '',
      password: ''
  }
];
const CAMERA_PORT = 8888;
const DISPLAY_PORT = 8889;
const HTTP_PORT = 80;
const password = 1234;
const cameraServer = new WebSocket.Server({ port: CAMERA_PORT }, () =>
  console.log(`Camera Server is listening at ${CAMERA_PORT}`)
);

const clientServer = new WebSocket.Server({ port: DISPLAY_PORT }, () =>
console.log(`Client Server is listening at ${DISPLAY_PORT}`)
);
http.listen(80,()=>{
  console.log(`Http Server is listening at ${HTTP_PORT}`)
});
class cihaz {
  constructor(mac) {
    this._mac = mac;
  }
  ws_ayarla(ws) {
    this._ws = ws;
  }
}
let cihazlar = [];
let connectedCameras = [];
let connectedClients = [];
app.use(bodyParser.urlencoded({ extended: true }));
const video_olustur = (isim) => {
  return new Promise((resolve, reject) => {
    var ffmpegPath = require("@ffmpeg-installer/ffmpeg").path;
    var mkdirp = require("mkdirp");
    mkdirp("./V" + isim, function (err) {
      if (err) {
        console.error(err);
        resolve(-1);
      } else {
        ffmpeg.setFfmpegPath(ffmpegPath);
        ffmpeg()
          .input("./F" + isim + "/test%d.jpg")
          .inputFPS(4)
          .output(
            "./V" +
              isim +
              "/G " +
              dateFormat(Date(), "yyyy-mm-dd h-MM-ss") +
              ".mp4"
          )
          .noAudio()

          .on("end", function () {
            console.log("downlaod resolved");
            resolve(1);
          })
          .on("error", function (err) {
            console.log(err);
            resolve(0);
          })
          .run();
      }
    });
  });
};
const video_baslat = async (indis) => {
  var sonuc2 = await video_olustur(cihazlar[indis]._mac);
  if (sonuc2 == 1) {
    const fs = require("fs-extra");
    fs.emptyDir("./F" + cihazlar[indis]._mac, (err) => {
      if (err) return console.error(err);
      console.log("success!");
    });
  }
};
const ws_sorgula = (ws) => {
  return new Promise((resolve, reject) => {
    var kontrol = 0;
    for (var t = 0; t < cihazlar.length; t++) {
      if (ws == cihazlar[t]._ws) {
        kontrol = 1;
        resolve(t);
      }
    }
    if (kontrol == 0) {
      resolve(-1);
    }
  });
};
cameraServer.on("connection", (ws, req) => {
  const foto_klasör_isim = "deneme";
  const video_klasör_isim = "videolar";

  ws.on("error", function (e) {
    console.log("Client error: %s", e.message);
  });

  console.log("Camera connected");

  console.log("camrelalar:" + connectedCameras.length);

  ws.on("message", async (data) => {
    giris++;
    if (data.length > 600) {
      var sonuc = await ws_sorgula(ws);
      if (sonuc != -1) {
        // console.log(cihazlar[sonuc]._mac);
        if (giris < -1) {
          console.log("buradayım");
          var buffer = new Buffer.from(data, "binary");
          var mkdirp = require("mkdirp");
          mkdirp("./F" + cihazlar[sonuc]._mac, function (err) {
            if (err) console.error(err);
            else {
              fs.writeFile(
                "./F" + cihazlar[sonuc]._mac + "/test" + giris + ".jpg",
                buffer,
                function (err, written) {
                  if (err) console.log(err);
                  else {
                    console.log("Successfully written");
                  }
                }
              );
            }
          });
        }

        connectedClients.forEach((ws, i) => {
          if (ws.readyState === ws.OPEN) {
            // console.log(data)
            ws.send(data);
          } else {
            connectedClients.splice(i, 1);
          }
        });
      }
    } else {
      console.log("şifreyi burada alabilirim");
      var json = JSON.parse(data);

      if (json.sifre == password) {
        var indis = null;

        for (var t = 0; t < cihazlar.length; t++) {
          if (json.mac == cihazlar[t]._mac) {
            indis = t;
            break;
          }
        }
        if (json.type == "video") {
          video_baslat(indis);
        } else {
          if (indis == null) {
            var yeni_cihaz = new cihaz(json.mac);
            yeni_cihaz.ws_ayarla(ws);
            cihazlar.push(yeni_cihaz);
          } else {
            cihazlar[indis].ws_ayarla(ws);
          }
        }
      }
    }
  });

  ws.on("close", (data) => {
    console.log("çıktıkii");
  });
});

clientServer.on("connection", (ws, req) => {
  //const ip = req.socket.remoteAddress;
  // console.log(ip)
  ws.on("error", function (e) {
    console.log("Client error: %s", e.message);
  });
  console.log("Client connected");
  connectedClients.push(ws);
});
const generateAuthToken = () => {
  return crypto.randomBytes(30).toString('hex');
}
app.use(cookieParser());
app.use((req, res, next) => {
  // Get auth token from the cookies
  const authToken = req.cookies['AuthToken'];
  //console.log(req)
  console.log("verdi "+authToken)

  // Inject the user to the request
  req.user = authTokens[authToken];

  next();
});
const requireAuth = (req, res, next) => {
  if (req.user) {
      next();
  } else {
    console.log("hadi len oradan")
    res.redirect("/login")
  }
};

app.get("/video", requireAuth, async (req, res) => {
  //console.log(req.socket.remoteAddress);
  if (req.query.adi == undefined) {
    res.writeHead(404, { "Content-Type": "text/html" });
    res.write("<h2>404 Not Found\n</h2>");
    res.end();
  } else {
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
});
app.get("/", requireAuth, (req, res) => {
  res.sendFile(path.resolve(__dirname, "./index.html"));
});
app.get("/login", (req, res) => {
  res.sendFile(path.resolve(__dirname, "./login.html"));
});
app.post("/login", (req, res) => {
  const { istek } = req.body
  if(istek=="cık"){
    const authToken = req.cookies['AuthToken'];
    authTokens[authToken] = null;
    res.json({sonuc:"basarılı"})
    console.log("çıkıcam")
  }
  
});
app.post('/dndfnhdfn', (req, res) => {
    const { email, password } = req.body;
        res.render('login', {
            message: 'Invalid username or password',
            messageClass: 'alert-danger'
        });
    
});
app.post("/io", (req, res) => {

  const { email, password } = req.body;
  
 
  const user = users[0];

if (user) {
  const authToken = generateAuthToken();

  // Store authentication token
  authTokens[authToken] = user;
console.log("oluşturdum "+authToken)
  // Setting the auth token in cookies
  res.cookie('AuthToken', authToken);

  // Redirect user to the protected page
 console.log(email+" "+password)
 res.sendFile(path.resolve(__dirname, "./io_deneme.html"));
}
});


app.get('/io', requireAuth, (req, res) => {
  res.sendFile(path.resolve(__dirname, "./io_deneme.html"));
});

io.of("/login").on("connection", (soket) => {
  console.log("login geldi")
  soket.on("yönlendir", (data) => {
    console.log("yönlendireceğimm")
    
  });
})
io.of("/io").on("connection", (soket) => {
  console.log("io'dayım")
var macım = "24-0A-C4-60-69-4C";
const testFolder = "./V" + macım + "/";
var veriler = [];

fs.readdir(testFolder, (err, files) => {
  files.forEach((file) => {
    veriler.push(file);
  });
  soket.emit("liste", veriler);
});


soket.on("tiklandi",(data) => {
  soket.emit("deneme", { sonuc: "?adi=./V" + macım + "/" + data });
  console.log(data);
});

soket.on("reset", () => {
  console.log("resetlendi");
  for (var t = 0; t < cihazlar.length; t++) {
    if (macım == cihazlar[t]._mac) {
      cihazlar[t]._ws.send("kamera resetle");
      video_baslat(t);
    }
  }
});

soket.on("disconnect", () => {
  console.log("io çıktı");
});
})
app.use(function(req, res, next) {
  res.writeHead(404, { "Content-Type": "text/html" });
  res.write("<h2>404 Not Found\n</h2>");
  res.end();
})
/*const server = http.createServer((req, res) => {
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
  
  var macım = "24-0A-C4-60-69-4C";
  const testFolder = "./V" + macım + "/";
  var veriler = [];
  fs.readdir(testFolder, (err, files) => {
    files.forEach((file) => {
      veriler.push(file);
    });
    soket.emit("liste", veriler);
  });
  soket.on("tiklandi", (data) => {
    soket.emit("deneme", { sonuc: "?adi=./V" + macım + "/" + data });
    console.log(data);
  });
  soket.on("reset", () => {
    console.log("resetlendi");
    for (var t = 0; t < cihazlar.length; t++) {
      if (macım == cihazlar[t]._mac) {
        cihazlar[t]._ws.send("kamera resetle");
        video_baslat(t);
      }
    }
  });
  soket.on("disconnect", () => {
    console.log("çıktı");
  });
});*/