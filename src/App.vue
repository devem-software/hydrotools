<script setup>
import { RouterLink, RouterView } from "vue-router";
import navComponent from './components/navigateComponent.vue';
</script>

<template>
  <header>
    <h1>Hidrotools</h1>
  </header>
  <navComponent></navComponent>

  <RouterView />
</template>

<style lang="scss">
@import "@/assets/base.css";
</style>
