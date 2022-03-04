<template>
    <div class="data-main">
        <pre v-for="(item, itemKey) in items" :key="itemKey">
        {{ itemKey }}
            <p> temperatura : {{ item[0] }} </p>
            <p> humedad : {{ item[1] }} </p>
        <!-- <p v-for='(i,k) in item' :key='k'>{{ new Date(parseInt(itemKey)).toLocaleString() }}: {{ i }}</p> -->
        </pre>
    </div>
</template>
<script>
import { db } from '../db.js'
import { onValue, ref } from 'firebase/database'

export default {

    setup() {
        let items = []

        const rf = ref(db, '/edwin-marroquin')

        onValue(rf, (snapshot) => {
            items = snapshot.val()
        })

        const parseData = (dt) => {
            // return /\{+(.*)+\}/g.exec(dt)[1].split(',').map((d) => parseFloat(d.split(':')[1]))
            return dt
        }

        return { items, parseData }
    }
}
</script>

<style lang="scss">
    .data-main {
        display:flex;
        flex-wrap:wrap;
        flex:1;
        overflow: hidden;
        pre {
            width: 100%;
        }
    }
</style>
