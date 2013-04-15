class SC.Views.Thing extends Backbone.View
  initialize: ->
    @template = _.template $('#thing-template').html()
    @listenTo @model, 'change', @render

  events:
    "change .event-selection": "updateEditorContent"
    "click .save": "save"

  render: ->
    json = @model.toJSON()
    @$el.html @template json
    # now add the event handler names to the event selection
    # we'll make the editor hold the first event
    if json.events?
      sel = @$el.find(".event-selection")
      for e of json.events
        console.log "event", e
        sel.append("<option value='#{e}'>#{e}</option>");

    later => 
      @renderEditor()
      eventVal = @$(".event-selection").val()
      @editor.setValue @model.events.get(eventVal)?.get("content")
      @editor.navigateTo(0,0)
      @editingEvent = eventVal
      
    @el

  save: (e) ->
    e.preventDefault()
    data =
      ident: {}

    el = $(@el)
    data.ident.name = el.find("input[name='ident.name']").val()
    data.ident.cap_name = el.find("input[name='ident.cap_name']").val()
    if @editingEvent?
      data.events = @model.get('events')
      data.events[@editingEvent] = @editor.getValue()
    # we need to gather all of the details as well
    @model.set data
    @model.save()

  updateEditorContent: ->
    eventVal = @$(".event-selection").val()
    if eventVal != @editingEvent
      events = @model.get('events')
      if @editingEvent?
        events[@editingEvent] = @editor.getValue()
        @model.set({ 'events': events })
      @editor.setValue events[eventVal]
      @editor.navigateTo(0,0)
      @editingEvent = eventVal

  renderEditor: ->
    if !@editor?
      @editor = ace.edit("thing-event-editor")
      @editor.setTheme("ace/theme/textmate")
      @editor.getSession().setMode("ace/mode/c_cpp")

$("#new-event-modal").find(".btn-primary").click (e) ->
  e.preventDefault()
  event_name = $("#new-event-modal").find("input[name='name']").val()
  view = SC.App.currentViews["#main"]
  thing = view?.model
  if thing?
    events = thing.get('events')
    if not events.hasOwnProperty(event_name)
      events[event_name] = ""
      thing.set({'events': events})
    view.$(".event-selection").append("<option value='#{event_name}'>#{event_name}</option>")
    view.$(".event-selection").val(event_name)
      
  $("#new-event-modal").modal('hide')
