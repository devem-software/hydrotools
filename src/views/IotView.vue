<template>
  <div class="map">
    <iframe
      id="map"
      :src="state.geoLoc"
      frameborder="0"
      style="width: 100%; height: auto"
    ></iframe>
  </div>
  <div class="home">
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
  setup() {
    const state = reactive({
      channels: {},
      data: [],
      info: [],
      lastData: [],
      geoLoc: "",
      loading: true,
      active: false,
    });

    const getData = async () => {
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

    const setTheme = () => {
      state.active = !state.active;
      document.body.classList.toggle("td");
      document.body.classList.toggle("tl");
    };

    onMounted(async () => {
      state.channels = channels;
      setInterval(getData, 10000);
      navigator.geolocation.getCurrentPosition((pos) => {
        state.geoLoc = `https://www.google.com/maps/embed?pb=!1m10!1m8!1m3!1d400!2d${pos.coords.longitude}!3d${pos.coords.latitude}!3m2!1i2048!2i768!4f13!5e1!3m2!1ses-419!2sco!4v1647399527212!5m2!1ses-419!2sco`;
      });
    });

    return { state, setTheme };
  },
};
</script>

<style lang="scss"></style>
