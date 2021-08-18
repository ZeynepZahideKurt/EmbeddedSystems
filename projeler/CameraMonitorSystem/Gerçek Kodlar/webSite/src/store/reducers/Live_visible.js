import { live_visible } from "../types";

const default_state = false;

const reducer = (state = default_state, action) => {
  switch (action.type) {
    case live_visible:
        
      return action.pyload;
     
    default:
      return state;
  }
};
export default reducer;