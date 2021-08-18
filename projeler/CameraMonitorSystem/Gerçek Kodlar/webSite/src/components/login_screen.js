import React, { useState,useEffect } from "react";
import Avatar from "@material-ui/core/Avatar";
import Button from "@material-ui/core/Button";
import CssBaseline from "@material-ui/core/CssBaseline";
import TextField from "@material-ui/core/TextField";
import { useSelector, useDispatch } from "react-redux";
import Typography from "@material-ui/core/Typography";
import { makeStyles } from "@material-ui/core/styles";
import Container from "@material-ui/core/Container";
import Snackbar from "@material-ui/core/Snackbar";
import MuiAlert from "@material-ui/lab/Alert";
import { login_screen_visible } from "../store/actions/Login";
import { define_user } from "../store/actions/User";
import { video_visibility } from "../store/actions/VideoScreen";
import { define_soket_key } from "../store/actions/Sokey_key";
import { mac_array_clean_function } from "../store/actions/Mac_Array";
import Cookies from 'universal-cookie';


function Alert(props) {
  return <MuiAlert elevation={6} variant="filled" {...props} />;
}

const useStyles = makeStyles((theme) => ({
  paper: {
    marginTop: theme.spacing(8),
    display: "flex",
    flexDirection: "column",
    alignItems: "center",
  },
  avatar: {
    margin: theme.spacing(1),
    backgroundColor: theme.palette.secondary.main,
  },
  form: {
    width: "100%",
    marginTop: theme.spacing(1),
  },
  submit: {
    margin: theme.spacing(3, 0, 2),
  },
}));


export default function Login() {
   const state = useSelector((state) => state);
  const [url] = useState("http://"+state.RUrl);
  const [Error_open, setError_open] = useState(false);
  const [userMail, setuserMail] = useState("");
  const [password, setPassword] = useState("");
  const [id] = useState("");
  const [mac, setmac] = useState("");
  const [type, settype] = useState("error");
  const [Error_message, setError_message] = useState("");
  const classes = useStyles();
 
  const dispatch = useDispatch();

  var user = {
  
    user_password: password,
    userMail: userMail,
    mac: mac,
  };
//sayfa kapandığında açık bir hata mesajı var ise kapatır
  useEffect(() => {
  
    return () => {
      setError_open(false);
    }
  }, [])
  
//login buton tıklandığında servera istek atarak login işlemini gerçekleştirmekle alaklı btün işi yapan fonksiyondur
  const log_in = async function () {

    try {
      
      fetch(url+"/api", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          type: "login",
          mail:"z.fatmasolak@gmail.com",
           sifre: "23",
         // mail:userMail,
        // sifre: password,
          device:"web" 
        }),
      })
        .then((response) => response.json())
        .then(async(json) => {
          console.log(json)
          //eğer login başarısız sonucu döndü ise hata mesajı göterilir    
          if (json.cookie === null) {
            setError_message(json.message);
            settype("error");
            Error_message_function();

          } 
          //eğer login isteğinin cevabı başarılı ise cookie ve sayfa geçişleri ayarlanır
          else if(json.cookie != null&&json.key!=null&&json.mac!=null) {
          try {
            const cookies = new Cookies();
           cookies.set('otomatik', json.cookie, { path: '/' });
           user = {
      
            user_password: password,
            userMail: userMail,
            mac: mac,
          };
      
          dispatch(define_user(user));
            let key_json={
              cookie:json.cookie,
              key:json.key,
            };

            dispatch(define_soket_key(key_json))
            dispatch(mac_array_clean_function());
            setError_message("Giriş Başarılı");
            settype("success");
            Error_message_function();
            dispatch(video_visibility(true));
            dispatch(login_screen_visible(false));
           
          } catch (error) {
            console.log(error)
          }
          }
        });
    } catch (error) {
      console.log(error);
    }
  };
  

// başarılı ve hata mesajlarından hangi olduğuna göre kapatma fonsiyonu yazılır
  const Error_message_function = () => {
  setError_open(true);
    if (id === "") {
      setTimeout(function () {
       setError_open(false);
      }, 2000);
    } 
  };

  return (
    <Container component="main" maxWidth="xs" >
      <CssBaseline />
      <div className={classes.paper}>
        <Avatar className={classes.avatar}></Avatar>
        <Typography component="h1" variant="h5">
          Giriş Yap
        </Typography>
        <form className={classes.form} noValidate>
          <TextField
            variant="outlined"
            margin="normal"
            required
            fullWidth
            id="email"
            label="Kullanıcı Adı "
            name="email"
            autoFocus
            onChange={(e) => setuserMail(e.target.value)}
            value={userMail}
          />
          <TextField
            variant="outlined"
            margin="normal"
            required
            fullWidth
            name="password"
            label="Şifre"
            type="password"
            id="password"
            value={password}
            onChange={(e) =>  setPassword(e.target.value)}
          />

          <Button
            type="button"
            fullWidth
            variant="contained"
            color="primary"
            onClick={(e) => log_in(e)}
            className={classes.submit}
          >
            Giriş Yap
          </Button>
        </form>
        <Snackbar
          anchorOrigin={{ vertical: "top", horizontal: "right" }}
          open={Error_open}
          autoHideDuration={6000}
        >
          <Alert severity={type}>{Error_message}</Alert>
        </Snackbar>
      </div>
    </Container>
  );
}
