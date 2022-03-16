import { createRouter, createWebHistory } from "vue-router";

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
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
});

export default router;
