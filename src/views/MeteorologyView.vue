<template>
    <h1>Vue 3 and Fetch Example</h1>
    <select name="channels" id="channels" @change="getData">
        <option selected>Seleccione un canal</option>
        <option v-for="(i, k) in state.channels" :key="k" :value="k">{{ i }}</option>
    </select>
    <ul>
        <li v-for="(post, k) in state.data" :key="k">
            {{ parseFloat(post["field1"]).toFixed(2) }}
            {{ parseFloat(post["field2"]).toFixed(2) }}
        </li>
    </ul>
    <p v-if="state.loading">Espere un momento . . .</p>
</template>

<script>
import channels from './../channels';
import { onMounted, reactive } from "vue";

export default {
    name: "MeteorologyPage",
    props: {},
    setup() {
        const state = reactive({
            channels: {},
            data: [],
            info: [],
            loading: true,
        });

        const getData = async (e) => {
            console.log(e)
            state.loading = true;
            state.data = [];
            const data = await fetch(
                "https://api.thingspeak.com/channels/1662545/feeds.json"
            );
            state.data = await data.json();
            state.info = state.data.channel;
            state.data = state.data.feeds;
            state.loading = false;
        }

        onMounted(async () => {
            state.channels = channels;
            setInterval(getData(),2000)
            
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
