import React from 'react';
import ReactDOM from 'react-dom';
import App from './App';
import { Provider } from 'react-redux'
import store from './store'
import reportWebVitals from './reportWebVitals';
import { setAuthorizationToken } from "./helpers/setAuthorizationToken.js";
const jwtToken = localStorage.getItem("jwtToken");
if (jwtToken) {
    setAuthorizationToken(jwtToken);
}

const rootElement = document.getElementById('root')
ReactDOM.render(
  <Provider store={store}>
    <App />
    </Provider>,
 rootElement
 
);

// If you want to start measuring performance in your app, pass a function
// to log results (for example: reportWebVitals(console.log))
// or send to an analytics endpoint. Learn more: https://bit.ly/CRA-vitals
reportWebVitals();
