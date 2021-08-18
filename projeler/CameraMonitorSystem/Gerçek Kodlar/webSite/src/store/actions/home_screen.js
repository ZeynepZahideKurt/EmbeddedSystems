import { home_visible } from "../types";

export const home_visibility = (gelen) => {
  return {
    type: home_visible,
    pyload: gelen,
  };
};