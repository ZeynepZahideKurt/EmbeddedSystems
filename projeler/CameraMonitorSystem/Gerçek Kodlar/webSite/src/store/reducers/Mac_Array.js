import { mac_array_add,mac_array_clean } from "../types";



const reducer = (state = [], action) => {
  switch (action.type) {
    case mac_array_add:
        
      return  [...state,action.pyload]
     case mac_array_clean:
        return [];
    default:
      return state;
  }
};
export default reducer;