import { define_url } from "../types";

export const Define_Url = (gelen) => {
  return {
    type: define_url,
    pyload: gelen,
  };
};