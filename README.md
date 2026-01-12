# CREDIS

<b><i>A lightweight  redis clone implemented from scratch.</i></b>
<p>This project focuses on core Redis concepts such as the RESP protocol, common data structures, persistence, expiration, and concurrent TCP client handling.</p>

## Features

<ol>
 <li><b>Protocol</b>
    <ul>
        <li>RESP (Redis Serialization Protocol)</li>
        <li>Supports Redis-style request/response formatting</li>
        <li>Compatible with standard Redis clients for basic commands</li>
    </ul>
  </li>

 <li><b>Supported Data Types & Commands</b>
    <ol type="a">
       <li><i>String Commands</i>
           <ul>
              <li>SET</li>
              <li>GET</li>
              <li>DEL</li>
              <li>EXISTS</li>
              <li>INCR </li>
              <li>DECR</li>
            </ul>
        </li>
        <li><i>Hash Commands</i>
            <ul>
               <li>HSET</li>
               <li>HGET</li>
               <li>HDEL</li>
               <li>HGETALL</li>
            </ul>
        </li>
        <li><i>Set Commands</i>
            <ul>
                <li>SADD</li>
                <li>SREM</li>
                <li>SMEMBERS</li>
                <li>SISMEMBER</li>
            </ul>
        </li>
        <li><i>List Commands</i>
            <ul>
               <li>LPUSH</li>
               <li>RPUSH</li>
               <li>LPOP</li>
               <li>RPOP</li>
               <li>LRANGE</li>
            </ul>
        </li>
        <li><i>Sorted Set (ZSet) Commands</i>
            <ul>
                <li>ZADD</li>
                <li>ZREM</li>
                <li>ZRANGE</li>
                <li>ZSCORE</li>
            </ul>
        </li>
        <li><i>Key Commands</i>
            <ul>
                <li>DEL</li>
                <li>EXPIRE</li>
                <li>TTL</li>
                <li>PERSIST</li>
                <li>RENAME</li>
            </ul>
        </li>
    </ol>
 </li>

 <li><b>Networking & Concurrency</b>
   <ul>
       <li>TCP server</li>
       <li>Thread-per-client model</li>
       <li>Each connected client is handled by a dedicated thread</li>
       <li>Supports multiple concurrent clients</li>
    </ul>
</li>
<li><b>Storage Engine</b>
    <ul>
        <li>In-memory data storage</li>
        <li>Optimized for fast reads and writes</li>
        <li>Data structures implemented internally (no external database dependency)</li>
    </ul>
</li>

<li><b>Persistence</b>
    <ul>
        <li>AOF (Append-Only File)</li>
        <li>All write operations are logged</li>
    </ul>
</li>

<li><b>Key Expiration</b>
    <ul>
        <li>Supports key expiration (TTL)</li>
        <li>Expired keys are automatically removed</li>
        <li>Expiration checked during access and/or via background cleanup</li>
    </ul>
</li>

</ol>


## Architecture Overview
<pre style="text-align:center">
Client
  ↓
TCP Server
  ↓
RESP Parser
  ↓
Engine
  ↓
Command Handler
  ↓
In-Memory Data Store
  ↓
AOF Persistence
</pre>

## Getting Started
## Example
<b>Build<b>
<pre>
 <code>
 cd build
 cmake ..
 make
</code>
</pre>

<b>run</b>
    <pre>
        <code>
        ./credis
        </code>
    </pre>
    
<i>
   <b>
      By default, the server listens on a configurable TCP port.
    </b>
</i>

<b>Usage</b>
   <p>
      You can connect by running the credis-client.py:
    </p>
<pre>
<code>
 python credis-client.py
</code>
</pre>

<b>Example session:</b>
    <pre>
        <code>
        SET key hello
        GET key
        </code>
    </pre>

## Limitations
  <ul>
        <li>Partial Redis command coverage</li>
        <li>No clustering or replication</li>
        <li>Thread-per-client model may not scale as efficiently as event-driven models</li>
        <li>Intended for learning and experimentation, not production use</li>
  </ul>

## Future Improvements
The following are the improvements to be made:
    <ul>
        <li>Event-driven I/O (epoll / kqueue)</li>
        <li>RDB snapshot support</li>
        <li>Enabling of data recovery on restart</li>
        <li>Replay of commands on startup to rebuild memory state</li>
        <li>Replication and leader–follower setup</li>
        <li>Lua scripting</li>
        <li>Improved memory eviction policies</li>
    </ul>
## Purpose
This project was built to:
    <ul>
        <li>Learn Redis internals</li>
        <li>Understand networking, concurrency, and protocol design</li>
        <li>Practice implementing core data structures and persistence mechanisms</li>
    </ul>

## License

MIT License 
