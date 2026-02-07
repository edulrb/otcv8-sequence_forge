filename = nil
loaded = false

function setFileName(name)
  filename = name
end

function isLoaded()
  return loaded
end

function load()
  local version = g_game.getClientVersion()
  local things = g_settings.getNode('things')

  local datPath, sprPath
  local otmlPath = "/data/things/" .. version .. "/Tibia"
  if things and things["data"] ~= nil and things["sprites"] ~= nil then
    datPath = resolvepath('/things/' .. things["data"])
    sprPath = resolvepath('/things/' .. things["sprites"])
  else
    if filename then
      datPath = resolvepath('/things/' .. filename)
      sprPath = resolvepath('/things/' .. filename)
    else
      datPath = resolvepath('/things/' .. version .. '/Tibia')
      sprPath = resolvepath('/things/' .. version .. '/Tibia')
    end
  end
  
    local sequencesPath = "/data/things/" .. version .. "/sequences.otml"

  local errorMessage = ''
  if not g_things.loadDat(datPath) then
    if not g_game.getFeature(GameSpritesU32) then
      g_game.enableFeature(GameSpritesU32)
      if not g_things.loadDat(datPath) then
        errorMessage = errorMessage .. tr("Unable to load dat file, please place a valid dat in '%s'", datPath) .. '\n'
      end
    else
      errorMessage = errorMessage .. tr("Unable to load dat file, please place a valid dat in '%s'", datPath) .. '\n'
    end
  end
  if not g_sprites.loadSpr(sprPath) then
    errorMessage = errorMessage .. tr("Unable to load spr file, please place a valid spr in '%s'", sprPath)
  end
  if not g_things.loadOtml(otmlPath) then
    errorMessage = errorMessage .. tr("Unable to load otml file, please place a valid otml in '%s'", otmlPath)
  end
  
    g_sequences.load(sequencesPath)

  loaded = (errorMessage:len() == 0)

  if errorMessage:len() > 0 then
    local messageBox = displayErrorBox(tr('Error'), errorMessage)
    addEvent(function() messageBox:raise() messageBox:focus() end)
    g_game.setClientVersion(0)
    g_game.setProtocolVersion(0)
  end
end

function playTestSequence()
  if not g_game.isOnline() then
    print('[!] >> Você precisa estar dentro do jogo para executar uma sequência.')
    return
  end

  -- Chama a sequência com ID 1 no seu personagem
  print('[+] >> Executando sequência de teste com ID 1.')
  playSequence(1)
end

-- [[ ALTERADO ]] -- O atalho Ctrl+D agora chama nossa nova função de teste
g_keyboard.bindKeyDown('Ctrl+Shift+D', playTestSequence)

function reloadOtml()
  local version = g_game.getClientVersion()
  if not version or version == 0 then
    print('[!] >> Falha ao recarregar: Versão do cliente não definida.')
    return
  end
  
  local sequencesPath = "/data/things/" .. version .. "/sequences.otml"
  local otmlPath = "/data/things/" .. version .. "/Tibia"
  g_sequences.load(sequencesPath)
  
  if g_things.loadOtml(otmlPath) then
    print('[+] >> OTML recarregado com sucesso!')
  else
    print('[!] >> Falha ao recarregar o arquivo OTML.')
  end
end

g_keyboard.bindKeyDown('Ctrl+D', reloadOtml)

    ProtocolGame.registerExtendedOpcode(209, function(protocol, opcode, buffer)
        print('[SERVER] >> Recebido comando para recarregar OTML.')
        reloadOtml()
    end)