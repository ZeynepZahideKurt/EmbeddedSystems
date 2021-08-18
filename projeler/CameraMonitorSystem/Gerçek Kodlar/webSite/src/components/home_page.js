import { w3cwebsocket as W3CWebSocket } from "websocket";
import React, { useState, useEffect, forwardRef } from "react";
import { Button, Paper, Typography, Grid } from "@material-ui/core";
import AppBar from "@material-ui/core/AppBar";
import { useSelector, useDispatch } from "react-redux";
import Toolbar from "@material-ui/core/Toolbar";
import { makeStyles } from "@material-ui/core/styles";
import {
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  TextField,
} from "@material-ui/core";
import Snackbar from "@material-ui/core/Snackbar";
import MuiAlert from "@material-ui/lab/Alert";
import { video_visibility } from "../store/actions/VideoScreen";
import { live_visibility } from "../store/actions/Live_visible";
import { login_screen_visible } from "../store/actions/Login";
import { home_visibility } from "../store/actions/home_screen";
import Cookies from "universal-cookie";
import MaterialTable, { MTableToolbar } from "material-table";
import AddBox from "@material-ui/icons/AddBox";
import ArrowDownward from "@material-ui/icons/ArrowDownward";
import Check from "@material-ui/icons/Check";
import ChevronLeft from "@material-ui/icons/ChevronLeft";
import ChevronRight from "@material-ui/icons/ChevronRight";
import Clear from "@material-ui/icons/Clear";
import DeleteOutline from "@material-ui/icons/DeleteOutline";
import Edit from "@material-ui/icons/Edit";
import FilterList from "@material-ui/icons/FilterList";
import FirstPage from "@material-ui/icons/FirstPage";
import LastPage from "@material-ui/icons/LastPage";
import Remove from "@material-ui/icons/Remove";
import SaveAlt from "@material-ui/icons/SaveAlt";
import ViewColumn from "@material-ui/icons/ViewColumn";
import LiveTvIcon from "@material-ui/icons/LiveTv";
import VideoLibraryIcon from "@material-ui/icons/VideoLibrary";
import $ from "jquery";

<script src="https://code.jquery.com/jquery-3.5.1.min.js"></script>;
<link
  rel="stylesheet"
  href="https://fonts.googleapis.com/icon?family=Material+Icons"
/>;
function Alert(props) {
  return <MuiAlert elevation={6} variant="filled" {...props} />;
}
const VideoElement = document.createElement("VIDEO");
const useStyles = makeStyles((theme) => ({
  paper: {
    margin: theme.spacing(1),
    width: 200,
    height: 400,
  },

  buttonContainer: {},
  title: {
    flexGrow: 1,
  },
  titleBar: {
    background:
      'linear-gradient(to bottom, rgba(0,0,0,0.7) 0%, ' +
      'rgba(0,0,0,0.3) 70%, rgba(0,0,0,0) 100%)',
  },
}));

const tableIcons = {
  Add: forwardRef((props, ref) => <AddBox {...props} ref={ref} />),
  Check: forwardRef((props, ref) => <Check {...props} ref={ref} />),
  Clear: forwardRef((props, ref) => <Clear {...props} ref={ref} />),
  Delete: forwardRef((props, ref) => <DeleteOutline {...props} ref={ref} />),
  DetailPanel: forwardRef((props, ref) => (
    <ChevronRight {...props} ref={ref} />
  )),
  Edit: forwardRef((props, ref) => <Edit {...props} ref={ref} />),
  Export: forwardRef((props, ref) => <SaveAlt {...props} ref={ref} />),
  Filter: forwardRef((props, ref) => <FilterList {...props} ref={ref} />),
  FirstPage: forwardRef((props, ref) => <FirstPage {...props} ref={ref} />),
  LastPage: forwardRef((props, ref) => <LastPage {...props} ref={ref} />),
  NextPage: forwardRef((props, ref) => <ChevronRight {...props} ref={ref} />),
  PreviousPage: forwardRef((props, ref) => (
    <ChevronLeft {...props} ref={ref} />
  )),
  ResetSearch: forwardRef((props, ref) => <Clear {...props} ref={ref} />),
  SortArrow: forwardRef((props, ref) => <ArrowDownward {...props} ref={ref} />),
  ThirdStateCheck: forwardRef((props, ref) => <Remove {...props} ref={ref} />),
  ViewColumn: forwardRef((props, ref) => <ViewColumn {...props} ref={ref} />),
  VideoRecord: forwardRef((props, ref) => <VideoLibraryIcon {...props} ref={ref} />),
};
//video elemnti bu kısımda oluşturuluyor
const video_load = (gelen) => {
  VideoElement.setAttribute("src", gelen);
  //x.onloadstart = function () {};
  VideoElement.setAttribute("width", "400");
  VideoElement.setAttribute("height", "400");
  VideoElement.setAttribute("controls", "controls");
  document.querySelector("#video").append(VideoElement);
};
const live_screen = (element) => {
  element.setAttribute("src", "");
  //x.onloadstart = function () {};
  element.setAttribute("width", "400");
  element.setAttribute("height", "400");
  $("#deneme").append(element);
};

export default function Home_page() {
  const classes = useStyles();
  const state = useSelector((state) => state);
  const dispatch = useDispatch();
  const [Password_change, setPassword_change] = useState(false);
  const [Old_password, setOld_password] = useState("");
  const [New_password, setNew_password] = useState("");
  const [Repeat_new_password, setRepeat_new_password] = useState("");
  const [VideoDialog, setVideoDialog] = useState(false);
  const [VideoArray, setVideoArray] = useState([]);
  const [VideoServer, setVideoServer] = useState();
  const [AlertOpen, setAlertOpen] = useState(false);
  const [Type, setType] = useState("error");
  const [Error_message, setError_message] = useState("");
  const [OneScreenLiveDialog, setOneScreenLiveDialog] = useState(false);
  const [MultipleScreenLiveDialog, setMultipleScreenLiveDialog] = useState(false);
  const [LiveServer, setLiveServer] = useState();
  const [LiveMacDevice, setLiveMacDevice] = useState([]);
  const [LiveDevice, setLiveDevice] = useState();
  const [OneMacName, setOneMacName] = useState("");
  useEffect(() => {
    const Vserver = new W3CWebSocket("ws:///" + state.RUrl + ":8887");
    setVideoServer(Vserver);
    Vserver.onopen = () => {
      console.log("WebSocket server Connected");
    };

    Vserver.onmessage = async (message) => {


      message = JSON.parse(message.data);
     // console.log(message);


      //gelen mesajın tipine göre
      switch (message.type) {
        //video listesi elemnetini değiştirme
        case "video_list_response":
          setVideoArray(...VideoArray, message.VideoList);
          setVideoDialog(true)
          break;

        //error mesajı vererek video listesini temizleme
        case "error_message":
          setError_message(message.message);
          setType("error");
          Error_message_function(1500);
          break;
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


    //MAc listesi elementine click listener eklenerek gerekli fonksiyon atanıyor

    return () => { };
  }, []);
  //kullanıcı çıkış yapmak istediğinde bu fonksiyon çalışır
  const log_out = () => {
    try {
      //servera log out isteği atılır
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
          }
          //eğer çıkış yapma isteği başarılı olmuş ise soket yapısı kapatılır ve oturum kapatılır
          else {
            const cookies = new Cookies();
            cookies.set(
              "otomatik",
              "vjd495vt849mokg5ytfmöfoe48tufmömvsvsv56s4v6sdlgmdbv1cs665s4vsğvössv4s6v46s",
              { path: "/" }
            );
            dispatch(login_screen_visible(true));
            dispatch(live_visibility(false));
            dispatch(video_visibility(false));
            dispatch(home_visibility(false));
            window.location.reload();
          }
        });
    } catch (error) {
      console.log(error);
    }
  };
  function OnClickMac(mac_name) {
    let json2 = {
      type: "DefineMacFileName",
      cookie: state.RKey_soket.cookie,
      key: state.RKey_soket.key,
      mac: mac_name,
    };
    VideoServer.send(JSON.stringify(json2));
  }
  const video_record_dialog_close_function = () => {
    //  console.log(Old_password);
    setVideoDialog(false);

  };
  const Error_message_function = (time) => {
    setAlertOpen(true);

    setTimeout(function () {
      setAlertOpen(false);
    }, time);
  };
  const One_screen_live_dialog_create_function = (Mac) => {
    const Lserver = new W3CWebSocket("ws:///" + state.RUrl + ":8889");
    setLiveServer(Lserver);
    Lserver.onopen = () => {
      console.log("live server Connected");
      let jsonData = [];
      let message = { message: "bağlan" };
      jsonData.push(message);
      jsonData.push(state.RKey_soket)
      setLiveDevice(Mac)
      jsonData.push([{ isim: "" + Mac + "" }]);
      setOneMacName(Mac)


      //soket yapısı ile bağlan mesajı ile kameraların açılması sağlanır
      Lserver.send(JSON.stringify(jsonData));
    };

    //json yapısını senkron olaraak parse eden fonksiyon
    const ParseJsonSynchronous = async (data) => {
      return new Promise((resolve) => {
        resolve((data = JSON.parse(data)));
      });
    };

    let urlObject;
    //serverdan gelen mesajlar burada yakalanır
    Lserver.onmessage = async (message) => {
      //eğer  sessionda bir skıntı varsa oturumun kapatılması sağlanır
      if (message.data === "log_out") {
        window.location.reload();
      }
      // cihazlardan bağlantısı kopanları kullanıcının anlayabilmesi için görüntü sıfırlanır
      else if (message.data == "deviceDisconnect") {
        document.querySelector("#OneLiveScreen").src = ""

      }
      //başarılı bir şekilde fotoğraf bilgisi geldiğinde kısıma geçer ve fotoğrafa dönüştürülecek ilgili elemente aktarılır
      else {

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
        document.querySelector("#OneLiveScreen").src = urlObject.toString()

      }
    };
    setOneScreenLiveDialog(true);


  };
  const One_screen_live_close_function = () => {
    let jsonData = [];
    let message = { message: "kop" };
    jsonData.push(message);
    jsonData.push(state.RKey_soket)
    let json = { isim: "" + LiveDevice + "" };
    jsonData.push(json);
    //kullanıcının kameralarına canlı yayını kapatması mesajı gönderilmesi için servera istek yollanır
    LiveServer.send(JSON.stringify(jsonData));
    LiveServer.close();
    setOneScreenLiveDialog(false);

  };
  const Multiple_screen_live_dialog_create_function = (MacArray) => {
    setMultipleScreenLiveDialog(true);
    const Lserver = new W3CWebSocket("ws:///" + state.RUrl + ":8889");
    setLiveServer(Lserver);
    Lserver.onopen = () => {
      console.log("live server Connected");
      let jsonData = [];
      let MacData = [];
      let message = { message: "bağlan" };
      jsonData.push(message);
      jsonData.push(state.RKey_soket)
      for (var e = 0; e < MacArray.length; e++) {
        let json = { isim: "" + MacArray[e].name + "" };
        setLiveDevice(MacArray[e].name);
        MacData.push(json);
        
        if (e % 3!=0) {
          $("#Row"+parseInt(e/3+1)).append(
            "   <th> <img id=M" + MacArray[e].name + " width=400 height=400/><p>" + MacArray[e].name + "</p>   </th>");
        }
        else{
          $("#t01").append(
            "   <tr id=Row"+Math.round(e%3+1)+"> <th> <img id=M" + MacArray[e].name + " width=400 height=400/><p>" + MacArray[e].name + "</p>   </th>  </tr>");
        }


      }
      jsonData.push(MacData)
    //  console.log(jsonData)
      //soket yapısı ile bağlan mesajı ile kameraların açılması sağlanır
      Lserver.send(JSON.stringify(jsonData));
    };

    //json yapısını senkron olaraak parse eden fonksiyon
    const ParseJsonSynchronous = async (data) => {
      return new Promise((resolve) => {
        resolve((data = JSON.parse(data)));
      });
    };

    let urlObject;
    //serverdan gelen mesajlar burada yakalanır
    Lserver.onmessage = async (message) => {

      //eğer  sessionda bir skıntı varsa oturumun kapatılması sağlanır
      if (message.data === "log_out") {
        window.location.reload();
      }
      // cihazlardan bağlantısı kopanları kullanıcının anlayabilmesi için görüntü sıfırlanır
      else if (message.data == "deviceDisconnect") {
        // document.querySelector("#OneLiveScreen").src = ""

      }
      //başarılı bir şekilde fotoğraf bilgisi geldiğinde kısıma geçer ve fotoğrafa dönüştürülecek ilgili elemente aktarılır
      else {

        let sonuc = await ParseJsonSynchronous(message.data);
        // console.log(new Blob([new Uint8Array(sonuc.blob.data,  sonuc.uzunluk,sonuc.uzunluk)]));

        if (urlObject) {
          URL.revokeObjectURL(urlObject);
        }
        //gelen blob mesajı bir orl objesine dönüştürülür
        urlObject = URL.createObjectURL(
          new Blob([new Uint8Array(sonuc.blob.data, sonuc.uzunluk)])
        );


        $("#M" + sonuc.mac).attr("src", urlObject.toString())


      }
    };



  };

  const Multiple_screen_live_close_function = (Mac) => {
    let jsonData = [];
    let message = { message: "kop" };
    jsonData.push(message);
    jsonData.push(state.RKey_soket)

    let json = { isim: "" + LiveDevice + "" };
    jsonData.push(json);


    //kullanıcının kameralarına canlı yayını kapatması mesajı gönderilmesi için servera istek yollanır
    LiveServer.send(JSON.stringify(jsonData));
    LiveServer.close();

    setMultipleScreenLiveDialog(false);

  };
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
    <div>
      <AppBar position="static">
        <Toolbar>
          <Typography variant="h6" className={classes.title}>
            Enelsis
          </Typography>
          <Button onClick={password_dialog} color="inherit">
            Şifre Değiştir
          </Button>

          <Button onClick={log_out} color="inherit">
            Çıkış Yap
          </Button>
        </Toolbar>
      </AppBar>
      <MaterialTable
        icons={tableIcons}
        title=""
        columns={[{ title: "Cihaz Mac Adresleri", field: "name" }]}
        data={state.Rmac_array.map((element) => {
          return { name: element };
        })}
        options={{
          selection: true,
          search: false,
          rowStyle: {
            fontSize: 24,
          },
          headerStyle: {
            fontSize: 24,
          },
        }}
        //onSelectionChange={(rows) => console.log(rows)}
        actions={[
          {
            tooltip: "Canlı Yayın İzle",
            icon: () => <LiveTvIcon />,
            onClick: (evt, data) => {
              let MacArr = [];
              for (var e = 0; e < data.length; e++) {
                MacArr.push(data[e].name)
              }

              setLiveMacDevice(...LiveMacDevice, MacArr)
              Multiple_screen_live_dialog_create_function(data);
            },
          },
          {
            icon: () => <VideoLibraryIcon />,
            id: "MacArrayElement",
            tooltip: "Video Kayıtlarını İzle",
            position: "row",
            onClick: (event, rowData) => {
              OnClickMac(rowData.name)

            },
          },
          {
            tooltip: "Canlı Yayın İzle",
            icon: () => <LiveTvIcon />,
            position: "row",
            onClick: (evt, data) => {

              One_screen_live_dialog_create_function(data.name);
            },
          },
        ]}
        localization={{
          body: {
            emptyDataSourceMessage: "Gösterilecek kayıt yok",
          },
          toolbar: {
            nRowsSelected: "{0} Tane Satır Seçildi",
            searchTooltip: "Arama",
          },
          header: {
            actions: "Olaylar",
          },
          pagination: {
            labelRowsSelect: "satır",
            labelDisplayedRows: "{count} satırdan {from}-{to} arası",
            firstTooltip: "İlk Sayfa",
            previousTooltip: "Önceki Sayfa",
            nextTooltip: "Sonraki Sayfa",
            lastTooltip: "Son Sayfa",
          },
        }}
      />
      {VideoDialog && (<Dialog
        open={VideoDialog}

        aria-labelledby="form-dialog-title"
        fullWidth={true}
        maxWidth={"lg"}
      >

        <DialogContent>
          <Grid container spacing={1}>
            <Grid item xs={12} sm={6}>
              <Grid
                className={classes.Array}
                container
                justify="space-around"
                alignItems="baseline"
                direction="column-reverse"
              >
                <MaterialTable

                  icons={tableIcons}
                  title=""
                  columns={[{ title: "Video Tarihleri", field: "name" }]}
                  onRowClick={(event, rowData) => {
                    let json = {
                      type: "show_video",
                      cookie: state.RKey_soket.cookie,
                      key: state.RKey_soket.key,
                      video: rowData.name,
                    };
                    if (VideoServer !== undefined) {
                      VideoServer.send(JSON.stringify(json));
                    }
                  }
                  }

                  data={VideoArray.map((element) => {
                    return { name: element.substring(2, element.length - 4) };
                  })}
                  options={{
                    search: false,
                    rowStyle: {
                      fontSize: 24,
                    },
                    headerStyle: {
                      fontSize: 24,
                    },
                  }}

                  localization={{
                    body: {
                      emptyDataSourceMessage: "Gösterilecek kayıt yok",
                    },
                    toolbar: {
                      nRowsSelected: "{0} Tane Satır Seçildi",
                      searchTooltip: "Arama",
                    },

                    pagination: {
                      labelRowsSelect: "satır",
                      labelDisplayedRows: "{count} satırdan {from}-{to} arası",
                      firstTooltip: "İlk Sayfa",
                      previousTooltip: "Önceki Sayfa",
                      nextTooltip: "Sonraki Sayfa",
                      lastTooltip: "Son Sayfa",
                    },
                  }}
                />
              </Grid>
            </Grid>

            <Grid item xs={12} sm={6}>
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
        </DialogContent>
        <DialogActions>
          <Button onClick={video_record_dialog_close_function} color="primary">
            Kapat
          </Button>

        </DialogActions>
      </Dialog>)}
      {OneScreenLiveDialog && (<Dialog
        open={OneScreenLiveDialog}

        aria-labelledby="form-dialog-title"
        fullWidth={true}
      // maxWidth={"lg"}
      > 

        <DialogContent>
          <Paper
            id="video"
            variant="elevation"
            elevation={2}
            style={{ paddingTop: "2%", width: "100%" }}
          > <img id={"OneLiveScreen"} width="100%" height="400"></img>
            <Typography>{OneMacName}</Typography>
          </Paper>

        </DialogContent>
        <DialogActions>
          <Button onClick={One_screen_live_close_function} color="primary">
            Kapat
          </Button>

        </DialogActions>
      </Dialog>)}
      {MultipleScreenLiveDialog && (<Dialog
        open={MultipleScreenLiveDialog}
        aria-labelledby="form-dialog-title"
        fullWidth={true}
        maxWidth={"lg"}
      >

        <DialogContent >
          <table id="t01">
            

          </table>

        </DialogContent>
        <DialogActions>
          <Button onClick={Multiple_screen_live_close_function} color="primary">
            Kapat
          </Button>

        </DialogActions>
      </Dialog>)}
      {AlertOpen && (<Snackbar
        anchorOrigin={{ vertical: "top", horizontal: "right" }}
        open={AlertOpen}
        autoHideDuration={6000}
      >
        <Alert severity={Type}>{Error_message}</Alert>
      </Snackbar>)},
      {Password_change && (<Dialog open={Password_change} aria-labelledby="form-dialog-title">
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
      </Dialog>)}
    </div>
  );
}
