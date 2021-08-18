import { define_url } from "../types";

//const default_state = "89.19.23.98";
const default_state = "192.168.1.239";
const reducer = (state = default_state, action) => {
  switch (action.type) {
    case define_url:
        
      return action.pyload;
     
    default:
      return state;
  }
};
export default reducer;