import { user } from "../types";

const default_state = {
      user_mail:"",
      user_id:"",
      user_sifre:"",
      user_mac:[]
     
};

const reducer = (state = default_state, action) => {
  
  switch (action.type) {
    case user:
        
      return action.pyload;
     
    default:
      return state;
  }
};
export default reducer;