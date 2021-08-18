import { mac_array_add,mac_array_clean } from "../types";


export const mac_array_add_funciton = (gelen) => {
    return {
      type: mac_array_add,
      pyload: gelen,
    };
  };
  export const mac_array_clean_function = () => {
   
    return {
      type: mac_array_clean,
      pyload: false,
    };
  };