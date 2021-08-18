import React, { useState, useEffect } from "react";
import Cookies from 'universal-cookie';
import LiveScreen from "./components/live_screen";
import { home_visibility } from "./store/actions/home_screen";
import Video from "./components/video_record_screen";
import Login from "./components/login_screen";
import Home from "./components/home_page"
import { useSelector, useDispatch } from "react-redux";
import { mac_array_add_funciton, mac_array_clean_function } from "./store/actions/Mac_Array";
import { video_visibility } from "./store/actions/VideoScreen";
import { live_visibility } from "./store/actions/Live_visible";
import { login_screen_visible } from "./store/actions/Login";
import { define_soket_key } from "./store/actions/Sokey_key";


const ParseJsonSynchronous = async (data) => {
  return new Promise((resolve) => {
    resolve((data = JSON.parse(data)));
  });
};

function App() {
  const state = useSelector((state) => state);
  const [Live_, setLive_] = useState(false);
  const [video_, setvideo_] = useState(false);
  const [login_, setlogin_] = useState(false);
  const [home_, sethome_] = useState(false);
  const dispatch = useDispatch();
  const [cookieValue, setcookieVlue] = useState("")

  //sayfaa ilk açıldığında cookie kısmında otomatik adında bir cookie varsa onu deşkene kaydediyor yoksa staatic bir değer atayarak otomatik cookiesini oluşturuyor
  const cookie_parser = async (data) => {
    return new Promise((resolve) => {
      try {
        setcookieVlue(document.cookie
          .split('; ')
          .find(row => row.startsWith('otomatik='))
          .split('=')[1])

        resolve(cookieValue);
      } catch (error) {
        const cookies = new Cookies();
        cookies.set('otomatik', '"v56465dg1d6g41vgsc1g5g451vg55g72cx875gfssv6ssmvsvsv56s4v6sdlgmdbv1cs665s4vsğvössv4s6v46s"', { path: '/' });
        resolve(cookies);
      }

    });

  };

  //login sayfasının durumu değiştikçe bu efect yapısı çalışılır ve session durumunu kontrol eder
  useEffect(() => {

    try {
      // sayfa açıldığında açık bir session olup olmadığını kontrol etmek üzere servera post isteği yolluyor
      async function session() {
        await cookie_parser();
        fetch("http://" + state.RUrl + "/session", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({
            type: "sessionCheck",
            cookie: document.cookie
              .split('; ')
              .find(row => row.startsWith('otomatik='))
              .split('=')[1]
          }),
          device: "web"
        })
          .then((response) => response.json())
          .then(async (json) => {

            //eğer cevap olumsuz geldi ise otomatik cookiesine static değer atanıyor ve login sayfasına geçiş yapılıyor
            if (json.result === false) {
              const cookies = new Cookies();
              cookies.set('otomatik', '"v56465dg1d6g41vgsc1g5g451vg55g72cx875gfssv6ssmvsvsv56s4v6sdlgmdbv1cs665s4vsğvössv4s6v46s"', { path: '/' });
              dispatch(live_visibility(false));
              dispatch(video_visibility(false));
              dispatch(home_visibility(false));
              dispatch(login_screen_visible(true));
              setLive_(false);
              setvideo_(false);
              sethome_(false);
              setlogin_(true);
            } else {
              //eğer session bulunursa gelen macler kullanılmak üzere redux yapına kayddediliyor ve video izleme sayfasına geçiş yapılıyor
              let MacJson = await ParseJsonSynchronous(json.macler);
              let key_json = {
                "cookie": json.cookie,
                "key": json.key,
              };
              
              dispatch(define_soket_key(key_json))
              dispatch(mac_array_clean_function());
              MacJson.map((obj) => {
                dispatch(mac_array_add_funciton(obj.mac));
              });
              dispatch(live_visibility(false));
              dispatch(video_visibility(true));
              dispatch(login_screen_visible(false));
              dispatch(home_visibility(true));
              setLive_(false);
             // setvideo_(true);
              sethome_(true)
              setlogin_(false);
            }

          });
      }
      session();
    } catch (error) {
      console.log(error);
    }

  }, [state.Rlogin]);
  //canlı yayın sayfasına geçişler ayarlanır
  useEffect(() => {
    if (state.Rlogin === false) {
      setLive_(state.RLive);
      setvideo_(!state.RLive);
    }
  }, [state.RLive]);
  return (
    <div>
      {login_ && <Login />}
      {video_ && <Video />}
      {Live_ && <LiveScreen />}
      {home_ && <Home/>}
    </div>
  );
}

export default App;
