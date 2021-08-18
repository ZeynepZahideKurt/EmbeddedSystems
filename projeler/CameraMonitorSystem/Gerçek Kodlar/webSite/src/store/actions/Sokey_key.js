import { soket_key,soket_key_temizle } from "../types";

export const define_soket_key = (gelen) => {
  return {
    type: soket_key,
    pyload: gelen,
  };
};
export const soket_keytemizle = () => {
  return {
    type: soket_key_temizle,
    pyload: null,
  };
};