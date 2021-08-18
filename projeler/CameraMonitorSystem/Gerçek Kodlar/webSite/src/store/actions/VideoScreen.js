import { video_visible } from "../types";

export const video_visibility = (gelen) => {
  return {
    type: video_visible,
    pyload: gelen,
  };
};