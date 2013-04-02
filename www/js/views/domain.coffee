SC.Views.Domain = Backbone.View.extend
  initialize: ->
    this.template = _.template $('#domain-template').html()
    this.listenTo this.model, 'change', this.render

  events:
    'click .save': 'save'

  render: ->
    json = this.model.toJSON()
    this.$el.html this.template json
    if this.model.isNew()
      this.$el.find("h1").text("New Domain")
    this.el

  save: (e) ->
    e.preventDefault()
    data =
      name: this.$('input[name="name"]').val()
      description: this.$('input[name="description"]').val()
      plan: this.$('textarea[name="plan"]').val()
    this.model.set data

    if this.model.isNew()
      SC.Collections.domains.create this.model,
        success: => console.log "Saved!"
    else
      this.model.save()

    false
