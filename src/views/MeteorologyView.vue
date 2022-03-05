<template>
  <div class="data-main">
    <code>
        <pre>{{ Object.values(items).length }}</pre>
    </code>
    <pre v-for="(item, itemKey) in items" :key="itemKey">
        {{ itemKey }}
            <p> temperatura : {{ item[0] }} </p>
            <p> humedad : {{ item[1] }} </p>
        <!-- <p v-for='(i,k) in item' :key='k'>{{ new Date(parseInt(itemKey)).toLocaleString() }}: {{ i }}</p> -->
        </pre>
  </div>
</template>
<script>
import { db } from "../db.js";
import * as firebase from "firebase/database";
import { onMounted, ref } from "vue";

export default {
  setup() {
    let items = [];

    const rf = firebase.    ref(db, "/edwin-marroquin");

    firebase.onValue(rf, (snapshot) => items = ref(snapshot.val()));

    return { items };
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
