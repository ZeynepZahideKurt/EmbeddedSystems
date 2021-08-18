import { w3cwebsocket as W3CWebSocket } from "websocket";

import React, { useState, useEffect } from "react";
import {
  Button,
  Typography,
  Paper,
  Grid,
  TextField,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
} from "@material-ui/core";
import Snackbar from "@material-ui/core/Snackbar";
import MuiAlert from "@material-ui/lab/Alert";
import AppBar from "@material-ui/core/AppBar";
import { useSelector, useDispatch } from "react-redux";
import Toolbar from "@material-ui/core/Toolbar";
import { makeStyles } from "@material-ui/core/styles";
import { live_visibility } from "../store/actions/Live_visible";
import { video_visibility } from "../store/actions/VideoScreen";
import Cookies from "universal-cookie";
import { login_screen_visible } from "../store/actions/Login";
import $ from "jquery";
<script src="https://code.jquery.com/jquery-3.5.1.min.js"></script>;

function Alert(props) {
  return <MuiAlert elevation={6} variant="filled" {...props} />;
}
const VideoElement = document.createElement("VIDEO");
const useStyles = makeStyles((theme) => ({
  root: {
    flexGrow: 1,
  },
  paper: {
    margin: theme.spacing(1),
    width: "40%",
    height: "20%",
  },
  buttonContainer: {},
  title: {
    flexGrow: 1,
  },
  listeici: {
    paddingTop: 5,
    paddingLeft: 15,
    paddingRight: 15,
    paddingBottom: 5,
  },
  Array: {
    paddingTop: "2%",
    paddingLeft: 10,
    paddingRight: 10,
  },
}));

//video elemnti bu kısımda oluşturuluyor
const video_load = (gelen) => {
  VideoElement.setAttribute("src", gelen);
  //x.onloadstart = function () {};
  VideoElement.setAttribute("width", "200");
  VideoElement.setAttribute("height", "200");
  VideoElement.setAttribute("controls", "controls");
  document.querySelector("#video").append(VideoElement);
};

//reduxta kayıtlı mac listesi ile mac listesi elemanı dolduruluyor
const MacElementFunction = (MacArray) => {
  MacArray.forEach((obj, i) => {
    $("#MacArrayElement").append(
      "<li key={" + obj.toString() + "}>" + obj + "</li>"
    );
  });
};

//serverdan gelen video listesi ile video listesi elemanı dolduruluyor
const VideoElementFunction = (VideoArray) => {
  $("#VideoArrayTypography").remove("#0");
  VideoArray.forEach((obj, i) => {
    $("#VideoArrayTypography").append(
      "<li key={" + i + "}>" + obj.substring(2, obj.length - 4) + "</li>"
    );
  });
};

export default function Video_record_screen() {
  const [AlertOpen, setAlertOpen] = useState(false);
  const [Type, setType] = useState("error");
  const [Error_message, setError_message] = useState("");
  const [Password_change, setPassword_change] = useState(false);
  const [Old_password, setOld_password] = useState("");
  const [New_password, setNew_password] = useState("");
  const [Repeat_new_password, setRepeat_new_password] = useState("");
  const classes = useStyles();
  const state = useSelector((state) => state);
  const dispatch = useDispatch();
  const [VideoServer, setVideoServer] = useState();


  useEffect(() => {
    MacElementFunction(state.Rmac_array);
  //server ile bağlantı kuruluyor
    const server = new W3CWebSocket("ws:///" + state.RUrl + ":8887");
    setVideoServer(server);
    server.onopen = () => {
      console.log("WebSocket server Connected");
    };
    server.onmessage = async (message) => {
      message = JSON.parse(message.data);
      console.log(message);
      //gelen mesajın tipine göre 
      switch (message.type) {
        //video listesi elemnetini değiştirme
        case "video_list_response":
          $("#VideoArrayTypography").empty();
          $("#video").empty();
          VideoElementFunction(message.VideoList);
          break;
          //error mesajı vererek video listesini temizleme
        case "error_message":
          $("#VideoArrayTypography").empty();
          $("#video").empty();
          setError_message(message.message);
          setType("error");
          Error_message_function(1500);
          break;
          //sessionu olmayan kullanıcının oturumunu kapatma
        case "log_out":
          setError_message("Giriş Yapınız");
          setType("error");
          Error_message_function(100);
          window.location.reload();
          break;
          //video oynatma işelemi için video elemntini değiştirme
        case "videoUrl":
          video_load("http://" + state.RUrl + "/video" + message.url);
          break;
        default:
          console.log(message);
      }
    };
    //mac listesinden bir eleman tıklandığında servera mesaj yollanarak hangi cihazda işlem yapılacağı belirtilir
    function OnClickMac(mac_name) {
      let json2 = {
        type: "DefineMacFileName",
        cookie: state.RKey_soket.cookie,
        key: state.RKey_soket.key,
        mac: mac_name,
      };
      server.send(JSON.stringify(json2));
    }
    
    // video listesi elemntine click listner ekleniyor ve tıklama eventi anında servera o videonun adı yollanarak video izleme işinin başlması sağlanıyor
    document
      .querySelector("#Video_array")
      .addEventListener("click", function (e) {
        let json = {
          type: "show_video",
          cookie: state.RKey_soket.cookie,
          key: state.RKey_soket.key,
          video: e.target.innerHTML,
        };
        server.send(JSON.stringify(json));
        // state.Rsoket.emit("show_video", json);
      });

    //MAc listesi elementine click listener eklenerek gerekli fonksiyon atanıyor
    document
      .querySelector("#MacArrayElement")
      .addEventListener("click", function (e) {
        OnClickMac(e.target.innerHTML);
      });

    return () => {
      
    };
  }, []);

  //canlı yayın sayfasına geçiş işleri tamamlanır
  const Live_screen = () => {
    VideoServer.close();
    dispatch(live_visibility(true));
    dispatch(video_visibility(false));
  };

//çıkış yapmk için servera istek atılırr ve başarılı ise gerekli sayfa geçişleri ile cookie sıfırlama yapılır
  const Log_out = () => {
    try {
      
      fetch("http://" + state.RUrl + "/api", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          type: "log_out",
          cookie: document.cookie
            .split("; ")
            .find((row) => row.startsWith("otomatik="))
            .split("=")[1],
        }),
      })
        .then((response) => response.json())
        .then(async (json) => {
          //let sonuc = await dönüştür(json);

          if (json.result === false) {
          } else {
            const cookies = new Cookies();
            cookies.set(
              "otomatik",
              "vjd98539tym8fuldsgfmho845urmjssv4ssv6ssmvsvsv56s4v6sdlgmdbv1cs665s4vsğvössv4s6v46s",
              { path: "/" }
            );
            VideoServer.close();
            dispatch(live_visibility(false));
            dispatch(video_visibility(false));
            dispatch(login_screen_visible(true));
            window.location.reload();
          }
        });
    } catch (error) {
      console.log(error);
    }
  };

  const Error_message_function= (time) => {
    setAlertOpen(true);

    setTimeout(function () {
      setAlertOpen(false);
    }, time);
  };

  //şifre değişikliği için servera istek yollar ve gelen sonucu Snack Barda gösteririr
  const Password_change_function = () => {
    console.log("şifre değişs")
    var Tbody = {
      type: "changePassword",
      newPassword: New_password,
      oldPassword: Old_password,
      cookie: state.RKey_soket.cookie,
      key: state.RKey_soket.key,
    };
    try {
      const body = Tbody;
      fetch("http://" + state.RUrl + "/api", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(body),
      })
        .then((response) => response.json())
        .then(async (json) => {
          //let sonuc = await dönüştür(json);
          console.log(json);
          //  console.log(json);
          if (json.success === true) {
            setError_message(json.message);
            setType("success");
            setAlertOpen(true);
            setTimeout(function () {
              setAlertOpen(false);
            }, 2000);

            setPassword_change(false);
          } else {
            setError_message(json.message);
            setType("error");
            setAlertOpen(true);
            setTimeout(function () {
              setAlertOpen(false);
            }, 2000);
            setPassword_change(false);
          }
        });
    } catch (error) {
      // console.log(error);
    }
  };

 // dialogda kapatma btonu tıklandığında gerekli değişkenleri sıfırlar
  const password_dialog_close_function = () => {
    //  console.log(Old_password);
    setPassword_change(false);
    setOld_password("");
    setRepeat_new_password("");
    setNew_password("");
  };

  const password_dialog = () => {
    setPassword_change(true)
  };
  //girilen yeni şifrelerin aynı olup olmadığını kontrol ederek, aynı ise servera istek atmak için password_change_function çağırılır
  // eğer farklı ise textfieldlar temizlenerek kullanıcı uyarılır
  const validate_password = () => {
    if (Repeat_new_password !== "" && New_password !== "") {
      if (Repeat_new_password === New_password) {
        // console.log("şifreler aynı");
        setOld_password("");
        setRepeat_new_password("");
        setNew_password("");
        Password_change_function();
      } else {
        // console.log("şifreler farklı");
        setOld_password("");
        setRepeat_new_password("");
        setNew_password("");
        setError_message("Şifreler Farklı");
        setType("error");
        setAlertOpen(true);
        setTimeout(function () {
          setAlertOpen(false);
        }, 2000);
      }
    } else {
      setError_message("Yeni Şifre veya ŞifreTekrarı Boş Bırakılamaz");
      setType("error");
      setAlertOpen(true);
      setTimeout(function () {
        setAlertOpen(false);
      }, 2000);
    }
  };
  return (
    <div className={classes.root}>
      <AppBar position="static" id="root">
        <Toolbar>
          <Typography variant="h6" className={classes.title}>
            Enelsis
          </Typography>
          <Button onClick={Live_screen} color="inherit">
            Canli Yayin
          </Button>
          <Button onClick={password_dialog} color="inherit">
            Şifre Değiştir
          </Button>
          <Button onClick={Log_out} color="inherit">
            Çıkış Yap
          </Button>
        </Toolbar>
      </AppBar>
      <Grid container spacing={1}>
        <Grid item xs={6} sm={4}>
          <Grid
            className={classes.Array}
            container
            justify="space-around"
            alignItems="baseline"
            direction="column-reverse"
          >
            <Paper id="macElementID" variant="elevation" elevation={2}>
              <ul>
                <Typography
                  id="MacArrayElement"
                  component="h3"
                  className={classes.listeici}
                  style={{ fontFamily: "Montserrat", color: "#37474F" }}
                  variant="h5"
                ></Typography>
              </ul>
            </Paper>
          </Grid>
        </Grid>

        <Grid item xs={6} sm={4}>
          <Grid
            className={classes.Array}
            container
            justify="space-around"
            alignItems="baseline"
            direction="column-reverse"
          >
            <Paper
              id="paper2"
              variant="elevation"
              elevation={2}
              style={{ maxHeight: 400, overflow: "auto" }}
            >
              <ul id="Video_array">
                <Typography
                  id="VideoArrayTypography"
                  component="h3"
                  className={classes.listeici}
                  style={{ fontFamily: "Montserrat", color: "#37474F" }}
                  variant="h5"
                ></Typography>
              </ul>
            </Paper>
          </Grid>
        </Grid>

        <Grid item xs={6} sm={3}>
          <Grid
            className={classes.Array}
            container
            justify="space-around"
            alignItems="baseline"
            direction="column-reverse"
          >
            <Paper
              id="video"
              variant="elevation"
              elevation={2}
              style={{ paddingTop: "2%", width: "100%" }}
            ></Paper>
          </Grid>
        </Grid>
      </Grid>
      <Snackbar
        anchorOrigin={{ vertical: "top", horizontal: "right" }}
        open={AlertOpen}
        autoHideDuration={6000}
      >
        <Alert severity={Type}>{Error_message}</Alert>
      </Snackbar>
      <Dialog open={Password_change} aria-labelledby="form-dialog-title">
        <DialogTitle id="form-dialog-title">
          Şifre Değişiklik Ekranı
        </DialogTitle>
        <DialogContent>
          <TextField
            autoFocus
            margin="dense"
            id="E_Ş"
            label="Şuanki Şifrenizi Giriniz"
            fullWidth
            onChange={(e) => setOld_password(e.target.value)}
            value={Old_password}
          />
          <TextField
            margin="dense"
            id="Y_Ş"
            label="Yeni Şifrenizi Giriniz"
            fullWidth
            onChange={(e) => setNew_password(e.target.value)}
            value={New_password}
          />
          <TextField
            margin="dense"
            id="YT_Ş"
            label="Yeni Şifrenizi Tekrar Giriniz"
            fullWidth
            onChange={(e) => setRepeat_new_password(e.target.value)}
            value={Repeat_new_password}
          />
        </DialogContent>
        <DialogActions>
          <Button onClick={password_dialog_close_function} color="primary">
            Vazgeç
          </Button>
          <Button onClick={validate_password} color="primary">
            Onayla
          </Button>
        </DialogActions>
      </Dialog>
    </div>
  );
}
