<template>
  <h1>Vue 3 and Fetch Example</h1>

  <ul>
    <li v-for="(post, k) in state.data" :key="k">
      {{ parseFloat(post["field1"]).toFixed(2) }}
      {{ parseFloat(post["field2"]).toFixed(2) }}
    </li>
  </ul>
  <p v-if="state.loading">Espere un momento . . .</p>
</template>

<script>
import { db } from "../db.js";
import * as firebase from "firebase/database";
import { onMounted, reactive } from "vue";

export default {
  name: "MeteorologyPage",
  props: {},
  setup() {
    const state = reactive({
      data: [],
      info: [],
      loading: true,
    });

    onMounted(async () => {
      setInterval(async () => {
        const data = await fetch(
          "https://api.thingspeak.com/channels/1662545/feeds.json"
        );
        console.log(data)
        state.data = await data.json();
        state.info = state.data.channel;
        state.data = state.data.feeds;
        state.loading = false;
      }, 5000);
    });

    return { state };
  },
};
</script>

<style lang="scss">
.data-main {
  display: flex;
  flex-wrap: wrap;
  flex: 1;
  overflow: hidden;
  pre {
    width: 100%;
  }
}
</style>
