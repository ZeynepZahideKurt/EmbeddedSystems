import { live_visible } from "../types";

export const live_visibility = (gelen) => {
  return {
    type: live_visible,
    pyload: gelen,
  };
};