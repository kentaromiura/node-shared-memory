{
  "targets": [
    {
      "target_name": "node_shared_memory",
      "sources": [ "node_shared_memory.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
