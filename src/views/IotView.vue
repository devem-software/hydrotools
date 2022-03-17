<template>
  <div :class="['mo', state.active ? 'moon' : 'sun']" id="mo" @click="setTheme">
    <div :class="['icn', state.active ? 'act' : '']" id="icn">
      <div class="f fas fa-lg fa-sun"></div>
      <div class="f fas fa-lg fa-moon"></div>
    </div>
  </div>
  <div class="map">
    <iframe id="map" :src="state.geoLoc" frameborder="0" style="width: 100%; height: auto"></iframe>
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
      setInterval(getData, 2000);
      navigator.geolocation.getCurrentPosition((pos) => {
        state.geoLoc = `https://www.google.com/maps/embed?pb=!1m10!1m8!1m3!1d400!2d${pos.coords.longitude}!3d${pos.coords.latitude}!3m2!1i2048!2i768!4f13!5e1!3m2!1ses-419!2sco!4v1647399527212!5m2!1ses-419!2sco`;
      });
    });

    return { state, setTheme };
  },
};
</script>

<style lang="scss">

:root {
  --bg: #fafafa;
  --cv: #f5f5f5;
  --clr: #616161;
  --ttl: #689f38;
  --bdr: #cfd8dc;
  /*400*/
  --te: #ffca28;
  --hu: #29b6f6;
  --wn: #7e57c2;
  --pre: #ec407a;
}

.td {
  --bg: #212121;
  --cv: #424242;
  --clr: #f5f5f5;
  --ttl: #aed581;
  --bdr: #212121;
  /*100*/
  --te: #ffe082;
  /*AMBER*/
  --hu: #81d4fa;
  /*tl BLUE*/
  --wn: #b39ddb;
  /*DEEP PUERPLE*/
  --pre: #f48fb1;
  /*PINK*/
}

a {
  text-decoration: none;
  color: var(--ttl);
}
html {
  max-height: 100vh;
  height: 100%;
  font-family: "Outfit", sans-serif, verdana;
  font-size: 16px;
}
body {
  max-height: 100vh;
  height: 100%;
  display: flex;
  color: var(--clr);
  flex-direction: column;
  background: var(--bg);
}
header {
  span {
    line-height: 90%;
    text-align: center;
    flex: 1;
    &:nth-child(3) {
      margin-top: 1rem;
    }
    &.u {
      font-size: 1.5rem;
      color: #eb0b00;
    }
    &.d {
      font-size: 1.5rem;
      color: #f9dc25;
    }
  }
  background: var(--cv);
  display: flex;
  width: 100%;
  flex-direction: column;
  padding: 0.5rem;
  align-items: center;
  z-index: 1;
  height: 7rem;
  font-weight: 900;
  padding: 1rem;
  color: var();
  box-shadow: 0 -4px 9px 4px rgba(0, 0, 0, 0.7);
}
main {
  display: flex;
  flex-flow: column;
  height: 100%;
  overflow-y: auto;
  flex: 1;
}
.map {
  width: auto;
  min-height: 19rem;
  display: flex;
  flex: 1;
  margin: 1rem;
}
.data {
  flex: 1;
  display: flex;
  flex-flow: row wrap;
  margin: 0 1rem 1rem 1rem;
}
.dt {
  display: flex;
  width: calc(100% / 3);
  font-size: 1rem;
  padding: 1.5rem;
  background: var(--cv);
  flex-direction: column;
  align-items: center;
  justify-content: center;
  &-i {
    font-size: 3rem;
    width: 5rem;
    height: 5rem;
    display: flex;
    justify-content: center;
    align-items: center;
  }
  &-t {
    flex: 2;
    font-size: 1.5rem;
    margin: 0 1rem;
    text-align: center;
    display: none;
  }
  &-v {
    font-weight: 900;
    height: 6rem;
    justify-content: center;
    display: flex;
    align-items: center;
    &.v {
      flex-direction: column;
    }
  }
  &-tg {
    width: 2rem;
    display: inline-flex;
    justify-content: end;
  }
}
.te {
  color: var(--te);
}
.hu {
  color: var(--hu);
}
.pr {
  color: var(--pre);
}
.wn {
  color: var(--wm);
}
.coffee {
  color: #ff6e40;
  margin-left: 0.5rem;
}
footer {
  background: var(--cv);
  display: flex;
  width: 100%;
  flex-direction: column;
  padding: 0.5rem;
  align-items: center;
  z-index: 1;
  height: 5rem;
  font-size: 0.75rem;
  margin-top: auto;
  box-shadow: 0 4px 9px 4px rgba(0, 0, 0, 0.7);
  display: flex;
  justify-content: space-evenly;
  align-items: center;
}
.mo {
  position: absolute;
  z-index: 2;
  top: 2.5rem;
  right: 2rem;
  display: flex;
  align-items: center;
  justify-content: center;
  width: 2rem;
  height: 2rem;
  overflow: hidden;
  box-shadow: 0px 0px 8px 0px rgba(0, 0, 0, 0.5);
  border-radius: 100rem;
  .btn {
    width: 2rem;
    height: 2rem;
    display: flex;
    justify-content: center;
    align-items: center;
    border-radius: 100rem;
    width: 2rem;
    height: 2rem;
    background: var(--cv);
    box-shadow: inset 0 0 0 1px var(--clr);
    position: absolute;
    left: 0;
    right: 2rem;
  }
  .f {
    width: 2rem;
    height: 2rem;
    display: flex;
    justify-content: center;
    align-items: center;
  }
  .icn {
    width: 4rem;
    height: 2rem;
    display: flex;
    position: absolute;
    left: 0;
    right: 2rem;
    transition: all 0.5s ease;
    &.act {
      left: -2rem;
    }
  }
}
.sun {
  color: #ffab00;
}
.moon {
  color: #2962ff;
}
@media screen and (min-width: 380px) {
  .dt {
    font-size: 1.5rem;
  }
  .d {
    flex-direction: row;
  }
  .dt-i {
    font-size: 4rem;
  }
}
@media screen and (min-width: 600px) {
  .dt-t {
    display: block;
  }
  .dt-i {
    font-size: 5rem;
  }
}
</style>
