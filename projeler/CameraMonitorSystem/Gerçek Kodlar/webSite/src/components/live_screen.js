import { w3cwebsocket as W3CWebSocket } from "websocket";
import React, { useState, useEffect } from "react";
import { Button, Paper, Typography, Grid } from "@material-ui/core";
import AppBar from "@material-ui/core/AppBar";
import { useSelector, useDispatch } from "react-redux";
import Toolbar from "@material-ui/core/Toolbar";
import { makeStyles } from "@material-ui/core/styles";
import { video_visibility } from "../store/actions/VideoScreen";
import { live_visibility } from "../store/actions/Live_visible";
import { login_screen_visible } from "../store/actions/Login";
import Cookies from 'universal-cookie';
<script src="https://code.jquery.com/jquery-3.5.1.min.js"></script>;
const useStyles = makeStyles((theme) => ({
  paper: {
    margin: theme.spacing(1),
    width: 400,
    height: 400,
  },
  
  
  buttonContainer: {},
  title: {
    flexGrow: 1,
  },
}));



export default function Live_screen() {
  const classes = useStyles();
  const state = useSelector((state) => state);
  const dispatch = useDispatch();
  const [soket, setsoket] = useState("");

  useEffect(() => {
    //web soket bağlantısı sağlanır
    const server = new W3CWebSocket("ws:///"+state.RUrl+":8889");
    setsoket(server);
    server.onopen = () => {
      console.log("WebSocket server Connected");
      let jsonData = [];
      let message = { message: "bağlan" };
      jsonData.push(message);
      jsonData.push(state.RKey_soket)
      for (var e = 0; e < state.Rmac_array.length; e++) {
        let json = { isim: "" + state.Rmac_array[e] + "" };
        jsonData.push(json);
      }
   
      //soket yapısı ile bağlan mesajı ile kameraların açılması sağlanır
      server.send(JSON.stringify(jsonData));
    };

    //json yapısını senkron olaraak parse eden fonksiyon
    const ParseJsonSynchronous = async (data) => {
      return new Promise((resolve) => {
        resolve((data = JSON.parse(data)));
      });
    };

    let urlObject;
    //serverdan gelen mesajlar burada yakalanır
    server.onmessage = async (message) => {
      //eğer  sessionda bir skıntı varsa oturumun kapatılması sağlanır
      if(message.data==="log_out"){
        window.location.reload();
      }
      else{
     
      let sonuc = await ParseJsonSynchronous(message.data);
      // console.log(new Blob([new Uint8Array(sonuc.blob.data,  sonuc.uzunluk,sonuc.uzunluk)]));
       
      if (urlObject) {
        URL.revokeObjectURL(urlObject);
      }
      //gelen blob mesajı bir orl objesine dönüştürülür
      urlObject = URL.createObjectURL(
        new Blob([new Uint8Array(sonuc.blob.data, sonuc.uzunluk)])
      );
     
     // burada her gelen mesaj ilgili kameranın olduğu kısıma yönlendirilir
      document.querySelector("#M" + sonuc.mac).src=urlObject.toString()
    
    }
    };
  }, []);

  //canlı yayın sayfasından video sayfasına geçiş yapılmak istendiğinde bu fonksiyon çalışır
  const Video_Screen = () => {
    dispatch(video_visibility(true));
    dispatch(live_visibility(false));
    let jsonData = [];
    let message = { message: "kop" };
    jsonData.push(message);
    jsonData.push(state.RKey_soket)
    for (var e = 0; e < state.Rmac_array.length; e++) {
      let json = { isim: "" + state.Rmac_array[e] + "" };
      jsonData.push(json);
    }

    //kullanıcının kameralarına canlı yayını kapatması mesajı gönderilmesi için servera istek yollanır
    soket.send(JSON.stringify(jsonData));
    soket.close();
  };

  //kullanıcı çıkış yapmak istediğinde bu fonksiyon çalışır
  const log_out = () => {
    try {
      //servera log out isteği atılır
      fetch("http://"+state.RUrl+"/api", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          type: "log_out",
          cookie:document.cookie
          .split('; ')
          .find(row => row.startsWith('otomatik='))
          .split('=')[1]
        }),
      })
        .then((response) => response.json())
        .then(async (json) => {
          //let sonuc = await dönüştür(json);
          
          if (json.result === false) {
          } 
          //eğer çıkış yapma isteği başarılı olmuş ise soket yapısı kapatılır ve oturum kapatılır
          else {
          const cookies = new Cookies();
           cookies.set('otomatik', "vjd495vt849mokg5ytfmöfoe48tufmömvsvsv56s4v6sdlgmdbv1cs665s4vsğvössv4s6v46s", { path: '/' })
           dispatch(login_screen_visible(true));
           soket.close();
           
           window.location.reload();
          }
        });
    } catch (error) {
      console.log(error);
      
    }
    
  };
  return (
    <div>
      <AppBar position="static">
        <Toolbar>
          <Typography variant="h6" className={classes.title}>
            Enelsis
          </Typography>
          <Button onClick={Video_Screen} color="inherit">
            Video Kaydı
          </Button>

          <Button onClick={log_out} color="inherit">
            Çıkış Yap
          </Button>
        </Toolbar>
      </AppBar>
      <Grid container spacing={2}>
        <Grid item xs={12}>
          <Grid container spacing={2}>
            {state.Rmac_array.map((obj, i) => {
              return (
                <Grid key={i} item>
                  <Paper  className={classes.paper}>
                    <img id={"M" + obj} width="400" height="400"></img>
                  </Paper>
                </Grid>
              );
            })}
          </Grid>
        </Grid>
      </Grid>
    </div>
  );
}
