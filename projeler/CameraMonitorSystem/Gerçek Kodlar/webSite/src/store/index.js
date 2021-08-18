import {createStore,combineReducers} from 'redux'
import LiveReducer from './reducers/Live_visible'
import VideoReducer from './reducers/VideoScreen'
import LoginReducer from './reducers/Login'
import HomeReducer from './reducers/home_screen'
import UserReducer from './reducers/User'
import Mac_listeReducer from'./reducers/Mac_Array'
import Soket_key_reducer from'./reducers/Soket_key'
import Url_reducer from './reducers//Define_url'

 const reducers=combineReducers({
     
    
     RLive:LiveReducer,
     Rvideo:VideoReducer,
     Ruser:UserReducer,
     Rlogin:LoginReducer,
     Rmac_array:Mac_listeReducer,
     RKey_soket:Soket_key_reducer,
     RUrl:Url_reducer,
     Rhome:HomeReducer,
     
 })
 const store=createStore(reducers)

 export default store