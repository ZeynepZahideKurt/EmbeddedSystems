import { login_screen } from "../types";



const reducer = (state = true, action) => {
  switch (action.type) {
    case login_screen:
        
      return action.pyload;
     
    default:
      return state;
  }
};
export default reducer;