<template>
  <div class="data">
    <LineChart
      ref="chartRef"
      :chartData="state.collecH"
      :options="optionsChart"
      class="chart"
    ></LineChart>
    <LineChart
      ref="chartRef"
      :chartData="state.collecT"
      :options="optionsChart"
      class="chart"
    ></LineChart>
  </div>
</template>

<script>
import { LineChart } from "vue-chart-3";
import { Chart, registerables } from "chart.js";
import { ref, onMounted, reactive } from "vue";

Chart.register(...registerables);

export default {
  name: "MeteorologyPage",
  props: {},
  components: {
    LineChart,
  },
  setup() {
    const state = reactive({
      data: [],
      info: [],
      x: [],
      yT: [],
      yH: [],
      collecT: null,
      collecH: null,
    });

    const data = ref();
    const chartRef = ref();

    const getData = async () => {
      state.data = [];
      const data = await fetch(
        "https://api.thingspeak.com/channels/1662545/feeds.json"
      );
      state.data = await data.json();
      state.info = state.data.channel;
      state.x = state.data.feeds.map((i) =>
        new Date(i.created_at).toLocaleDateString()
      );
      state.yT = state.data.feeds.map((i) => parseFloat(i.field1));
      state.yH = state.data.feeds.map((i) => parseFloat(i.field2));

      state.collecH = {
        labels: state.x,
        datasets: [
          {
            label: "Humedad",
            data: state.yH,
            backgroundColor: ["#689f38"],
            borderColor: "#689f38",
            borderWidth: 1,
            animations: false,
            cubicInterpolationMode: "monotone",
            tension: 4,
            hoverBackgroundColor: ["teal"],
          },
        ],
      };

      state.collecT = {
        labels: state.x,
        datasets: [
          {
            label: "Temperatura",
            data: state.yT,
            backgroundColor: ["#689f38"],
            borderColor: "#689f38",
            borderWidth: 1,
            animations: false,
            cubicInterpolationMode: "monotone",
            tension: 4,
            hoverBackgroundColor: ["teal"],
          },
        ],
      };
    };

    const optionsChart = {
      resposive: true,
      plugins: {
        legend: {
          display: false,
        },
      },
      scales: {
        x: {
          title: {
            display: true,
            text: "Fecha",
          },
        },
        y: {
          display: true,
        },
      },
    };

    const updateData = () => {
      setInterval(() => {
        getData();
      }, 1000);
    };

    onMounted(async () => {
      await getData();
      data.value = state.collecH;
      await updateData();
    });

    return { state, chartRef, optionsChart };
  },
};
</script>

<style lang="scss" scpoped>
.data {
  flex: 1;
  display: flex;
  flex-direction: column;
  justify-content: space-around;
  align-items: center;
  margin-top: 2rem;
  .chart {
    flex: 1;
    width: 100%;
  }
}
</style>
