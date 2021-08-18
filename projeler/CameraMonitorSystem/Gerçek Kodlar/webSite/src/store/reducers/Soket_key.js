import { soket_key,soket_key_temizle } from "../types";

const default_state = null;

const reducer = (state = default_state, action) => {
  switch (action.type) {
    case soket_key:
        
      return action.pyload;
    case soket_key_temizle:
        state=null;
        return null;
    default:
      return state;
  }
};
export default reducer;