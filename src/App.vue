<script>
import navComponent from "@/components/navigateComponent.vue";
import { reactive } from "vue";

export default {
  name: "MeteorologyPage",
  props: {},
  components: {
    navComponent,
  },
  setup() {
    const state = reactive({
      active: false,
    });

    const setTheme = () => {
      state.active = !state.active;
      document.body.classList.toggle("td");
      document.body.classList.toggle("tl");
    };

    return { state, setTheme };
  },
};
</script>

<template>
  <header>
    <div
      :class="['mo', state.active ? 'moon' : 'sun']"
      id="mo"
      @click="setTheme"
    >
      <div :class="['icn', state.active ? 'act' : '']" id="icn">
        <div class="f fas fa-lg fa-sun"></div>
        <div class="f fas fa-lg fa-moon"></div>
      </div>
    </div>
    <h1>Hidrotools</h1>
    <navComponent></navComponent>
  </header>

  <RouterView />
</template>

<style lang="scss">
@import "@/scss/base.css";
@import "@/scss/main.scss";
</style>
