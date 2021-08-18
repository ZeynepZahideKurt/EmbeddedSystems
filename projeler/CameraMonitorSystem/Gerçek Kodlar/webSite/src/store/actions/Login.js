import { login_screen } from "../types";

export const login_screen_visible = (gelen) => {
  return {
    type: login_screen,
    pyload: gelen,
  };
};