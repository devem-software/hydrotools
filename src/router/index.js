import { createRouter, createWebHistory } from "vue-router";

const basePath = document.getElementsByTagName("base")[0].href;

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  base: basePath,
  routes: [
    {
      path: "/",
      name: "home",
      component: () => import("../views/HomeView.vue"),
    },
    {
      path: "/iot",
      name: "iot",
      component: () => import("../views/IotView.vue"),
    },
    {
      path: "/statictis",
      name: "statictis",
      component: () => import("../views/StatictisView.vue"),
    },
  ],
})

export default router;
