import { createRouter, createWebHistory } from 'vue-router'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'home',
      component: () => import('../views/HomeView.vue')
    },
    {
      path: '/meteorology',
      name: 'meteorology',
      component: () => import('../views/MeteorologyView.vue')
    }
  ]
})

export default router
