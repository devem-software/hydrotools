<template>
  <h1>Vue 3 and Fetch Example</h1>
  <select name="channels" id="channels" @change="getData">
    <option selected>Seleccione un canal</option>
    <option v-for="(i, k) in state.channels" :key="k" :value="k">{{ i }}</option>
  </select>
  <div class="map">
    <iframe id="map" :src="geoLoc" frameborder="0" style="width: 100%; height: auto"></iframe>
  </div>
  <div class="data">
    <div class="dt">
      <div class="dt-i fad fa-calendar-clock"></div>
      <div class="dt-v v">
        <span id="fd">{{ state.lastData.time }}</span>
        <span id="ft">{{ state.lastData.date }}</span>
      </div>
    </div>
    <div class="dt">
      <div class="dt-i fad fa-temperature-sun te"></div>
      <div class="dt-v te">
        <span id="te">{{ state.lastData.field1 }}</span>
        <span class="dt-tg">°C</span>
      </div>
    </div>
    <div class="dt">
      <div class="dt-i fad fa-hand-holding-water hu"></div>
      <div class="dt-v hu">
        <span id="hu">{{ state.lastData.field2 }}</span>
        <span class="dt-tg">%</span>
      </div>
    </div>
  </div>
</template>

<script>
import channels from "../channels";
import { onMounted, reactive } from "vue";

export default {
  name: "MeteorologyPage",
  props: {},
  setup () {
    const state = reactive({
      channels: {},
      data: [],
      info: [],
      lastData: [],
      loading: true,
    });

    const getData = async (e) => {
      state.data = [];
      const data = await fetch(
        "https://api.thingspeak.com/channels/1662545/feeds.json"
      );
      state.data = await data.json();
      state.info = state.data.channel;
      state.data = state.data.feeds;
      state.lastData = state.data[state.data.length - 1];
      state.lastData["date"] = new Date(
        state.lastData.created_at
      ).toLocaleDateString();
      state.lastData["time"] = new Date(
        state.lastData.created_at
      ).toLocaleTimeString();
    };

    onMounted(async () => {
      navigator.geolocation.getCurrentPosition((pos) => {
        state.geoLoc = `https://www.google.com/maps/embed?pb=!1m10!1m8!1m3!1d600!2d${pos.coords.longitude}!3d${pos.coords.latitude}!3m2!1i2048!2i768!4f13!5e1!3m2!1ses-419!2sco!4v1647399527212!5m2!1ses-419!2sco`;
      })
      state.channels = channels;
      setInterval(getData, 2000);
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
