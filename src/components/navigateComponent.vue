<template>
  <nav>
    <ul>
      <li>
        <router-link to="/">Home</router-link>
      </li>
      <li>
        <router-link to="/iot">IOT</router-link>
      </li>
      <li>
        <router-link to="/statistics">Statistics</router-link>
      </li>
    </ul>
  </nav>
</template>

<script>
export default {

}
</script>

<style lang="scss" scoped>
nav {
  ul {
    height: 2rem;
    display: flex;
    sjustify-content: center;
    align-items: center;
    list-style: none;
    li {
      flex: 1;
      a {
        height: 2rem;
        display: flex;
        justify-content: center;
        align-items: center;
        background: var(--ttl);
        color: var(--bg);
        font-size: 1rem;
        font-weight: 900;
      }
    }
  }
}
</style>
